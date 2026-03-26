#include "LevelSelect.hpp"

#include <array>
#include <string>

#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "CameraManager.hpp"
#include "AudioManager.hpp"
#include "TextObject.hpp"
#include "Color.hpp"
#include "Utils.hpp"
#include "Config.hpp"
#include "UICursor.hpp"
#include "LevelManager.hpp"

namespace LevelSelect {

    namespace {
        constexpr int kTotalButtons = Config::TOTAL_LEVEL_COUNT;

        std::array<Button, kTotalButtons> levelButtons;
        Button backButton;

        TextObject hintText;

        AEGfxTexture* bgTexture = nullptr;
        AEGfxTexture* buttonTexture = nullptr;

        constexpr float kLevelButtonSize = 170.0f;
        constexpr float kLevelButtonGap = 50.0f;
        constexpr float kLevelButtonRowCenterY = 40.0f;

        AEVec2 GetLevelButtonTopLeft(int buttonIndex, int totalButtons, float buttonSize, float gap, float rowCenterY) {
            const float groupWidth = (totalButtons * buttonSize) + ((totalButtons - 1) * gap);
            const float startX = -groupWidth * 0.5f;
            const float topY = rowCenterY - (buttonSize * 0.5f);

            return {
                startX + buttonIndex * (buttonSize + gap),
                topY
            };
        }

        GameStateManager::GameState LevelIndexToState(int levelIndex) {
            return static_cast<GameStateManager::GameState>(
                static_cast<int>(GameStateManager::GameState::LEVEL_1) + levelIndex
                );
        }

        void OpenLevel(int levelIndex) {
            if (!LevelManager::IsLevelUnlocked(levelIndex)) {
                return;
            }

            GameStateManager::next = LevelIndexToState(levelIndex);
        }

        void ConfigureLevelButton(Button& button, int levelIndex, AEVec2 topLeftPos, float buttonSize) {
            const bool unlocked = LevelManager::IsLevelUnlocked(levelIndex);

            std::string label = std::to_string(levelIndex + 1);

            Color buttonColor(0.46f, 0.46f, 0.50f, 1.0f);
            Color textColor(0.20f, 0.20f, 0.22f, 1.0f);

            if (unlocked) {
                buttonColor = Color(0.88f, 0.92f, 0.62f, 1.0f);
                textColor = Color(0.16f, 0.16f, 0.16f, 1.0f);
            }

            button = Button(
                TextObject{ label, 1.55f, textColor },
                Utils::AnchorMode::CENTER,
                topLeftPos,
                { buttonSize, buttonSize },
                buttonColor,
                AssetsManager::squareTexture,
                nullptr,
                nullptr
            );
        }
    } // close anonymous namespace

    float PixelToNdcX(float pixelX) {
        return pixelX / (Config::SCREEN_WIDTH * 0.5f);
    }

    float PixelToNdcY(float pixelY) {
        return pixelY / (Config::SCREEN_HEIGHT * 0.5f);
    }

    void DrawCenteredTextPx(const std::string& text, float centerPixelX, float pixelY, float scale, const Color& color) {
        f32 textW = 0.0f;
        f32 textH = 0.0f;
        AEGfxGetPrintSize(AssetsManager::textFont, text.c_str(), scale, &textW, &textH);

        AEGfxPrint(
            AssetsManager::textFont,
            text.c_str(),
            PixelToNdcX(centerPixelX) - (textW * 0.5f),
            PixelToNdcY(pixelY),
            scale,
            color.Red(),
            color.Green(),
            color.Blue(),
            color.Alpha()
        );
    }

    void Load() {
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");
        buttonTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");
    }

    void Initialize() {
        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        for (int i = 0; i < kTotalButtons; ++i) {
            AEVec2 buttonTopLeft = GetLevelButtonTopLeft(
                i,
                kTotalButtons,
                kLevelButtonSize,
                kLevelButtonGap,
                kLevelButtonRowCenterY
            );

            ConfigureLevelButton(levelButtons[i], i, buttonTopLeft, kLevelButtonSize);
        }

        levelButtons[0].SetOnClick(OnLevel1Clicked);
        levelButtons[1].SetOnClick(LevelManager::IsLevelUnlocked(1) ? OnLevel2Clicked : nullptr);
        levelButtons[2].SetOnClick(LevelManager::IsLevelUnlocked(2) ? OnLevel3Clicked : nullptr);

        backButton = Button(
            TextObject{ "< BACK" },
            Utils::AnchorMode::CENTER,
            { -125.0f, -250.0f },
            { 250.0f, 75.0f },
            Color(0.82f, 0.28f, 0.28f, 1.0f),
            buttonTexture,
            nullptr,
            OnBackClicked
        );

        hintText = {
            "Grey buttons are locked   |   [1-3] Select unlocked level   |   ESC / BACK = Main Menu",
            0.60f,
            Color(0.0f, 0.0f, 0.0f, 0.95f),
            { -0.95f, -0.92f }
        };
    }

    void Draw() {
        if (bgTexture != nullptr) {
            GraphicsManager::DrawWorldUI(
                bgTexture,
                GraphicsManager::CreateTransformMatrix(
                    -Config::SCREEN_WIDTH / 2.0f,
                    Config::SCREEN_HEIGHT / 2.0f,
                    Config::SCREEN_WIDTH,
                    Config::SCREEN_HEIGHT),
                Color(1.0f, 1.0f, 1.0f, 1.0f)
            );
        }
        else {
            AEGfxSetBackgroundColor(0.12f, 0.12f, 0.16f);
        }

        const std::string progressLabel =
            "Currently unlocked: LEVEL " + std::to_string(LevelManager::HighestUnlockedLevel() + 1);

        DrawCenteredTextPx(
            "SELECT LEVEL",
            0.0f,
            285.0f,
            1.7f,
            Color(0.0f, 0.0f, 0.0f, 1.0f)
        );

        DrawCenteredTextPx(
            "Complete a level to unlock the next one",
            0.0f,
            225.0f,
            0.85f,
            Color(0.0f, 0.0f, 0.0f, 0.90f)
        );

        DrawCenteredTextPx(
            progressLabel,
            0.0f,
            185.0f,
            0.75f,
            Color(0.0f, 0.0f, 0.0f, 0.95f)
        );

        hintText.Draw();
    }

    void Update() {
        if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_BACK)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnBackClicked();
            return;
        }

        for (int i = 0; i < Config::TOTAL_LEVEL_COUNT; ++i) {
            if (AEInputCheckTriggered(static_cast<u8>(AEVK_1 + i)) &&
                LevelManager::IsLevelUnlocked(i)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OpenLevel(i);
                return;
            }
        }

        Draw();

        for (int i = 0; i < kTotalButtons; ++i) {
            if (LevelManager::IsLevelUnlocked(i)) {
                levelButtons[i].Update();
            }
            else {
                levelButtons[i].Draw();
            }
        }

        backButton.Update();
        UICursor::Draw();
    }

    void Free() {}

    void Unload() {}

    void OnLevel1Clicked() { OpenLevel(0); }
    void OnLevel2Clicked() { OpenLevel(1); }
    void OnLevel3Clicked() { OpenLevel(2); }

    void OnBackClicked() {
        GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
    }
}