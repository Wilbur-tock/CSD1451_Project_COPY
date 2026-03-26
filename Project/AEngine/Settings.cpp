#include "Settings.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "AudioManager.hpp"
#include "UIManager.hpp"
#include "CollisionManager.hpp"
#include "Utils.hpp"
#include "CameraManager.hpp"
#include "Config.hpp"
#include "UICursor.hpp"

namespace Settings {

    float currentMasterVolume = 0.5f;
    float currentBgmVolume = 1.0f;
    float currentSfxVolume = 1.0f;

    Button backButton;

    Button masterVolumeUpButton;
    Button masterVolumeDownButton;

    Button bgmVolumeUpButton;
    Button bgmVolumeDownButton;

    Button sfxVolumeUpButton;
    Button sfxVolumeDownButton;

    AEGfxTexture* bgTexture = nullptr;
    AEGfxTexture* btnTexture = nullptr;

    TextObject titleText;
    TextObject hintText;

    static const float SLIDER_W = 400.0f;
    static const float SLIDER_H = 30.0f;
    static const float SLIDER_LEFT_X = -SLIDER_W / 2.0f;

    static const float MASTER_Y = 70.0f;
    static const float BGM_Y = -30.0f;
    static const float SFX_Y = -130.0f;

    enum class DragTarget {
        None,
        Master,
        BGM,
        SFX
    };

    static DragTarget currentDragTarget = DragTarget::None;

    static float VolumeToKnobX(float volume) {
        return SLIDER_LEFT_X + volume * SLIDER_W;
    }

    static float WorldXToVolume(float worldX) {
        float t = (worldX - SLIDER_LEFT_X) / SLIDER_W;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return t;
    }

    static bool IsMouseOverSlider(float worldX, float worldY, float sliderY) {
        const float hitPad = 30.0f;
        return worldX >= SLIDER_LEFT_X - hitPad &&
            worldX <= SLIDER_LEFT_X + SLIDER_W + hitPad &&
            worldY >= sliderY - SLIDER_H - hitPad &&
            worldY <= sliderY + SLIDER_H + hitPad;
    }

    static void ApplyVolumesToAudio() {
        AudioManager::SetVolume(currentMasterVolume);
        AudioManager::SetBGMVolume(currentBgmVolume);
        AudioManager::SetSFXVolume(currentSfxVolume);
    }

    static void DrawSliderRow(const char* label, float sliderY, float value) {
        // Label
        float labelX = SLIDER_LEFT_X - 220.0f;
        float labelY = sliderY + 10.0f;
        Utils::NormalizeCoordinates(labelX, labelY);

        TextObject labelText = {
            label,
            0.9f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { labelX, labelY }
        };
        labelText.Draw();

        // Slider track
        GraphicsManager::DrawWorldUI(
            nullptr,
            GraphicsManager::CreateTransformMatrix(
                SLIDER_LEFT_X, sliderY,
                SLIDER_W, SLIDER_H),
            Color(0.2f, 0.2f, 0.2f, 0.5f)
        );

        // Slider fill
        float fillW = value * SLIDER_W;
        if (fillW > 1.0f) {
            GraphicsManager::DrawWorldUI(
                nullptr,
                GraphicsManager::CreateTransformMatrix(
                    SLIDER_LEFT_X, sliderY,
                    fillW, SLIDER_H),
                Color(0.2f, 0.6f, 0.9f, 0.75f)
            );
        }

        // Knob
        float knobX = VolumeToKnobX(value);
        float knobSz = 44.0f;
        GraphicsManager::DrawWorldUI(
            nullptr,
            GraphicsManager::CreateTransformMatrix(
                knobX - knobSz / 2.0f, sliderY + 7.0f,
                knobSz, knobSz),
            Color(1.0f, 1.0f, 1.0f, 1.0f)
        );

        float innerSz = 22.0f;
        GraphicsManager::DrawWorldUI(
            nullptr,
            GraphicsManager::CreateTransformMatrix(
                knobX - innerSz / 2.0f, sliderY,
                innerSz, innerSz),
            Color(0.2f, 0.6f, 0.9f, 1.0f)
        );

        // Percentage text
        int pct = static_cast<int>(value * 100.0f + 0.5f);
        std::string pctStr = std::to_string(pct) + "%";

        float pctX = SLIDER_LEFT_X + SLIDER_W + 70.0f;
        float pctY = sliderY + 25.0f;
        Utils::NormalizeCoordinates(pctX, pctY);

        TextObject pctText = TextObject{
            pctStr,
            0.9f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { pctX, pctY }
        };
        pctText.Draw();
    }

    static void AdjustVolume(float& value, float delta) {
        value += delta;
        if (value < 0.0f) value = 0.0f;
        if (value > 1.0f) value = 1.0f;
        ApplyVolumesToAudio();
    }

    void Load() {
        bgTexture = AssetsManager::GetTexture("Assets/game_bg1.png");
        btnTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");
    }

    void Initialize() {
        currentMasterVolume = AudioManager::GetVolume();
        currentBgmVolume = AudioManager::GetBGMVolume();
        currentSfxVolume = AudioManager::GetSFXVolume();

        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        float btnW = 220.0f, btnH = 70.0f;
        AEVec2 backPos = { -btnW / 2.0f, -250.0f };
        backButton = Button(
            TextObject{ "< BACK" },
            Utils::AnchorMode::CENTER,
            backPos,
            { btnW, btnH },
            Color(0.8f, 0.2f, 0.2f, 1.0f),
            btnTexture,
            nullptr,
            OnBackClicked
        );

        float smallBtnW = 60.0f, smallBtnH = 50.0f;
        float leftButtonX = -SLIDER_W / 2.0f - smallBtnW - 10.0f - smallBtnW / 2.0f;
        float rightButtonX = SLIDER_W / 2.0f + 10.0f + smallBtnW / 2.0f;

        masterVolumeDownButton = Button(
            TextObject{ "-" },
            Utils::AnchorMode::CENTER,
            { leftButtonX, MASTER_Y + 8.0f },
            { smallBtnW, smallBtnH },
            Color(0.7f, 0.3f, 0.3f, 1.0f),
            btnTexture,
            nullptr,
            OnMasterVolumeDownClicked
        );

        masterVolumeUpButton = Button(
            TextObject{ "+" },
            Utils::AnchorMode::CENTER,
            { rightButtonX, MASTER_Y + 8.0f },
            { smallBtnW, smallBtnH },
            Color(0.3f, 0.7f, 0.3f, 1.0f),
            btnTexture,
            nullptr,
            OnMasterVolumeUpClicked
        );

        bgmVolumeDownButton = Button(
            TextObject{ "-" },
            Utils::AnchorMode::CENTER,
            { leftButtonX, BGM_Y + 8.0f },
            { smallBtnW, smallBtnH },
            Color(0.7f, 0.3f, 0.3f, 1.0f),
            btnTexture,
            nullptr,
            OnBGMVolumeDownClicked
        );

        bgmVolumeUpButton = Button(
            TextObject{ "+" },
            Utils::AnchorMode::CENTER,
            { rightButtonX, BGM_Y + 8.0f },
            { smallBtnW, smallBtnH },
            Color(0.3f, 0.7f, 0.3f, 1.0f),
            btnTexture,
            nullptr,
            OnBGMVolumeUpClicked
        );

        sfxVolumeDownButton = Button(
            TextObject{ "-" },
            Utils::AnchorMode::CENTER,
            { leftButtonX, SFX_Y + 8.0f },
            { smallBtnW, smallBtnH },
            Color(0.7f, 0.3f, 0.3f, 1.0f),
            btnTexture,
            nullptr,
            OnSFXVolumeDownClicked
        );

        sfxVolumeUpButton = Button(
            TextObject{ "+" },
            Utils::AnchorMode::CENTER,
            { rightButtonX, SFX_Y + 8.0f },
            { smallBtnW, smallBtnH },
            Color(0.3f, 0.7f, 0.3f, 1.0f),
            btnTexture,
            nullptr,
            OnSFXVolumeUpClicked
        );

        titleText = {
            "SETTINGS",
            2.5f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { -0.17f, 0.70f }
        };

        hintText = {
            "Drag any slider, or use + / -  |  Top = Master, Middle = BGM, Bottom = SFX  |  ESC - Back",
            0.65f,
            Color(0.0f, 0.0f, 0.0f, 0.85f),
            { -0.98f, -0.93f }
        };
    }

    void Draw() {
        if (bgTexture) {
            GraphicsManager::DrawWorldUI(
                bgTexture,
                GraphicsManager::CreateTransformMatrix(
                    -Config::SCREEN_WIDTH / 2.0f, Config::SCREEN_HEIGHT / 2.0f,
                    Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT),
                Color(1.0f, 1.0f, 1.0f, 1.0f)
            );
        }
        else {
            AEGfxSetBackgroundColor(0.1f, 0.1f, 0.15f);
        }

        titleText.Draw();
        hintText.Draw();

        DrawSliderRow("MASTER", MASTER_Y, currentMasterVolume);
        DrawSliderRow("BGM", BGM_Y, currentBgmVolume);
        DrawSliderRow("SFX", SFX_Y, currentSfxVolume);
    }

    void Update() {
        AEVec2 savedCamPos = CameraManager::camera.GetPosition();
        CameraManager::camera.SetPosition({ 0.0f, 0.0f });

        if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_BACK)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnBackClicked();
            CameraManager::camera.SetPosition(savedCamPos);
            return;
        }

        float worldX = 0.0f, worldY = 0.0f;
        Utils::GetWorldMousePos(worldX, worldY);

        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            if (IsMouseOverSlider(worldX, worldY, MASTER_Y)) {
                currentDragTarget = DragTarget::Master;
            }
            else if (IsMouseOverSlider(worldX, worldY, BGM_Y)) {
                currentDragTarget = DragTarget::BGM;
            }
            else if (IsMouseOverSlider(worldX, worldY, SFX_Y)) {
                currentDragTarget = DragTarget::SFX;
            }
        }

        if (AEInputCheckReleased(AEVK_LBUTTON)) {
            currentDragTarget = DragTarget::None;
        }

        if (AEInputCheckCurr(AEVK_LBUTTON)) {
            float newValue = WorldXToVolume(worldX);

            switch (currentDragTarget) {
            case DragTarget::Master:
                currentMasterVolume = newValue;
                ApplyVolumesToAudio();
                break;
            case DragTarget::BGM:
                currentBgmVolume = newValue;
                ApplyVolumesToAudio();
                break;
            case DragTarget::SFX:
                currentSfxVolume = newValue;
                ApplyVolumesToAudio();
                break;
            default:
                break;
            }
        }

        Draw();

        backButton.Update();

        masterVolumeDownButton.Update();
        masterVolumeUpButton.Update();

        bgmVolumeDownButton.Update();
        bgmVolumeUpButton.Update();

        sfxVolumeDownButton.Update();
        sfxVolumeUpButton.Update();

        UICursor::Draw();
        CameraManager::camera.SetPosition(savedCamPos);
    }

    void Free() {
        currentDragTarget = DragTarget::None;
    }

    void Unload() {}

    void OnBackClicked() {
        currentDragTarget = DragTarget::None;

        if (Utils::isSettingsOpen) {
            Utils::isSettingsOpen = false;
            return;
        }

        GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
    }

    void OnMasterVolumeUpClicked() {
        AdjustVolume(currentMasterVolume, 0.1f);
    }

    void OnMasterVolumeDownClicked() {
        AdjustVolume(currentMasterVolume, -0.1f);
    }

    void OnBGMVolumeUpClicked() {
        AdjustVolume(currentBgmVolume, 0.1f);
    }

    void OnBGMVolumeDownClicked() {
        AdjustVolume(currentBgmVolume, -0.1f);
    }

    void OnSFXVolumeUpClicked() {
        AdjustVolume(currentSfxVolume, 0.1f);
    }

    void OnSFXVolumeDownClicked() {
        AdjustVolume(currentSfxVolume, -0.1f);
    }
}