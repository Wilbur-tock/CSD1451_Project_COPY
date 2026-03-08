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

namespace Settings {

    // State 
    float currentVolume = 0.5f;          // Default 50 %

    Button backButton;
    Button volumeUpButton;
    Button volumeDownButton;

    AEGfxTexture* bgTexture = nullptr;
    AEGfxTexture* btnTexture = nullptr;
    AEGfxTexture* sliderBgTex = nullptr; // nullptr -> drawn as solid rect

    TextObject titleText;
    TextObject volumeLabel;
    TextObject hintText;

    // Slider geometry (world coords, centred screen)
    static const float SLIDER_W = 400.0f;
    static const float SLIDER_H = 30.0f;
    static const float SLIDER_Y = -30.0f;   // vertical centre of slider
    static const float SLIDER_LEFT_X = -SLIDER_W / 2.0f;

    // Dragging state
    bool  isDragging = false;
    float dragOffsetX = 0.0f;   // unused - we set value directly from cursor X

    // Helpers 
    // Convert volume [0,1] -> slider knob X (world)
    static float VolumeToKnobX() {
        return SLIDER_LEFT_X + currentVolume * SLIDER_W;
    }

    // Convert world X -> volume [0,1], clamped
    static float WorldXToVolume(float worldX) {
        float t = (worldX - SLIDER_LEFT_X) / SLIDER_W;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return t;
    }

    // Lifecycle 
    void Load() {
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");
        btnTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");
    }

    void Initialize() {
        currentVolume = AudioManager::GetVolume();
        // Reset camera to origin
        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        // Back button
        float btnW = 220.0f, btnH = 70.0f;
        AEVec2 backPos = { -btnW / 2.0f, -160.0f };
        backButton = Button(
            TextObject{ "< BACK" },
            Utils::AnchorMode::CENTER,
            backPos,
            { btnW, btnH },
            Color(0.8f, 0.2f, 0.2f, 1.0f),
            btnTexture,
            OnBackClicked
        );

        // Volume +/- buttons (beside the slider) 
        float smallBtnW = 60.0f, smallBtnH = 50.0f;
        float btnY = SLIDER_Y;

        AEVec2 volDownPos = { -SLIDER_W / 2.0f - smallBtnW - 10.0f - smallBtnW / 2.0f, btnY };
        volumeDownButton = Button(
            TextObject{ "-" },
            Utils::AnchorMode::CENTER,
            volDownPos,
            { smallBtnW, smallBtnH },
            Color(0.7f, 0.3f, 0.3f, 1.0f),
            btnTexture,
            OnVolumeDownClicked
        );

        AEVec2 volUpPos = { SLIDER_W / 2.0f + 10.0f + smallBtnW / 2.0f, btnY };
        volumeUpButton = Button(
            TextObject{ "+" },
            Utils::AnchorMode::CENTER,
            volUpPos,
            { smallBtnW, smallBtnH },
            Color(0.3f, 0.7f, 0.3f, 1.0f),
            btnTexture,
            OnVolumeUpClicked
        );

        // Text objects 
        titleText = {
            "SETTINGS",
            2.5f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { -0.2f, 0.65f }
        };

        volumeLabel = {
            "VOLUME",
            1.0f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { -0.08f, 0.1f }
        };

        hintText = {
            "Click & drag the slider, or use + / -  |  ESC - Back",
            0.7f,
            Color(0.0f, 0.0f, 0.0f, 0.8f),
            { -0.98f, -0.93f }
        };
    }

    void Draw() {
        // Background
        if (bgTexture) {
            GraphicsManager::DrawWorldUI(
                bgTexture,
                GraphicsManager::CreateTransformMatrix(
                    -Config::SCREEN_WIDTH / 2.0f, Config::SCREEN_HEIGHT / 2.0f, 0,
                    Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT),
                Color(1.0f, 1.0f, 1.0f, 1.0f)
            );
        }
        else {
            AEGfxSetBackgroundColor(0.1f, 0.1f, 0.15f);
        }

        titleText.Draw();
        volumeLabel.Draw();
        hintText.Draw();

        // Slider track (dark background)
        GraphicsManager::DrawWorldUI(
            nullptr,
            GraphicsManager::CreateTransformMatrix(
                SLIDER_LEFT_X, SLIDER_Y, 0,
                SLIDER_W, SLIDER_H),
            Color(0.2f, 0.2f, 0.2f, 0.8f)
        );

        // Slider fill (coloured portion = current volume)
        float fillW = currentVolume * SLIDER_W;
        if (fillW > 1.0f) {
            GraphicsManager::DrawWorldUI(
                nullptr,
                GraphicsManager::CreateTransformMatrix(
                    SLIDER_LEFT_X, SLIDER_Y, 0,
                    fillW, SLIDER_H),
                Color(0.2f, 0.6f, 0.9f, 1.0f)
            );
        }

        // Slider knob
        float knobX = VolumeToKnobX();
        float knobSz = 44.0f;
        GraphicsManager::DrawWorldUI(
            nullptr,
            GraphicsManager::CreateTransformMatrix(
                knobX - knobSz / 2.0f, SLIDER_Y, 0,
                knobSz, knobSz),
            Color(1.0f, 1.0f, 1.0f, 1.0f)
        );
        // Inner knob accent
        float innerSz = 22.0f;
        GraphicsManager::DrawWorldUI(
            nullptr,
            GraphicsManager::CreateTransformMatrix(
                knobX - innerSz / 2.0f, SLIDER_Y, 0,
                innerSz, innerSz),
            Color(0.2f, 0.6f, 0.9f, 1.0f)
        );

        // Percentage label 
        int pct = static_cast<int>(currentVolume * 100.0f + 0.5f);
        std::string pctStr = std::to_string(pct) + "%";
        TextObject pctText = {
            pctStr,
            0.9f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { -0.04f, -0.12f }
        };
        pctText.Draw();
    }

    void Update() {
        // ESC -> back
        if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_BACK)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnBackClicked();
        }

        // Slider mouse interaction 
        // Get cursor world position directly
        float worldX = 0.0f, worldY = 0.0f;
        Utils::GetWorldMousePos(worldX, worldY);

        // Hit-test: is cursor over the slider track + a bit of vertical padding?
        float hitPad = 30.0f;
        bool overSlider =
            worldX >= SLIDER_LEFT_X - hitPad &&
            worldX <= SLIDER_LEFT_X + SLIDER_W + hitPad &&
            worldY >= SLIDER_Y - SLIDER_H - hitPad &&
            worldY <= SLIDER_Y + SLIDER_H + hitPad;

        if (AEInputCheckTriggered(AEVK_LBUTTON) && overSlider) {
            isDragging = true;
        }
        if (AEInputCheckReleased(AEVK_LBUTTON)) {
            isDragging = false;
        }

        if (isDragging && AEInputCheckCurr(AEVK_LBUTTON)) {
            currentVolume = WorldXToVolume(worldX);
            AudioManager::SetVolume(currentVolume);
        }

        Draw();

        // Buttons draw themselves
        backButton.Update();
        volumeUpButton.Update();
        volumeDownButton.Update();
    }

    void Free() {
        isDragging = false;
    }

    void Unload() {}

    // Button callbacks
    void OnBackClicked() {
        // ---- NEW: return to pause menu if we came from one ----     // <-- NEW
        if (Utils::returnToPause) {                                    // <-- NEW
            // Go back to the gameplay state we came from              // <-- NEW
            GameStateManager::next = GameStateManager::previous;       // <-- NEW
            // isPaused will be set to true after the state loads      // <-- NEW
            // (handled in GameStateManager via the returnToPause flag)// <-- NEW
            return;                                                    // <-- NEW
        }                                                              // <-- NEW
        // -----------------------------------------------------------// <-- NEW

        GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
    }

    void OnVolumeUpClicked() {
        currentVolume += 0.1f;
        if (currentVolume > 1.0f) currentVolume = 1.0f;
        AudioManager::SetVolume(currentVolume);
    }

    void OnVolumeDownClicked() {
        currentVolume -= 0.1f;
        if (currentVolume < 0.0f) currentVolume = 0.0f;
        AudioManager::SetVolume(currentVolume);
    }
}