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
        constexpr int kTotalButtons = LevelManager::TOTAL_LEVEL_COUNT;

        std::array<Button, kTotalButtons> levelButtons;
        Button backButton;

        TextObject titleText;
        TextObject subtitleText;
        TextObject progressText;
        TextObject hintText;

        AEGfxTexture* bgTexture = nullptr;
        AEGfxTexture* buttonTexture = nullptr;

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

        void ConfigureLevelButton(Button& button, int levelIndex, float x, float y, float width, float height) {
            const bool unlocked = LevelManager::IsLevelUnlocked(levelIndex);

            std::string label = std::to_string(levelIndex + 1);

            Color buttonColor(0.50f, 0.50f, 0.54f, 1.0f); // locked default
            if (unlocked) {
                buttonColor = Color(0.88f, 0.92f, 0.62f, 1.0f); // pale yellow-green like level-select style
            }

            button = Button(
                TextObject{ label, 1.5f, Color(0.18f, 0.18f, 0.18f, 1.0f) },
                Utils::AnchorMode::CENTER,
                { x, y },
                { width, height },
                buttonColor,
                buttonTexture,
                nullptr,
                nullptr
            );
        }
    }

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

        const float btnWidth = 180.0f;
        const float btnHeight = 180.0f;
        const float spacing = 215.0f;
        const float rowY = 40.0f;

        // Shift the whole 3-button group slightly left side
        const float groupCenterX = -100.0f;

        const float leftX = groupCenterX - spacing;
        const float centerX = groupCenterX;
        const float rightX = groupCenterX + spacing;

        ConfigureLevelButton(levelButtons[0], 0, leftX, rowY, btnWidth, btnHeight);
        levelButtons[0].SetOnClick(OnLevel1Clicked);

        ConfigureLevelButton(levelButtons[1], 1, centerX, rowY, btnWidth, btnHeight);
        levelButtons[1].SetOnClick(LevelManager::IsLevelUnlocked(1) ? OnLevel2Clicked : nullptr);

        ConfigureLevelButton(levelButtons[2], 2, rightX, rowY, btnWidth, btnHeight);
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


        titleText = {
             "SELECT LEVEL",
             1.7f,
             Color(0.0f, 0.0f, 0.0f, 1.0f),
             { 0.0f, 0.0f }
        };

        subtitleText = {
            "Complete a level to unlock the next one",
            0.85f,
            Color(0.0f, 0.0f, 0.0f, 0.90f),
            { 0.0f, 0.0f }
        };

        progressText = {
            "Currently unlocked: LEVEL " + std::to_string(LevelManager::HighestUnlockedLevel() + 1),
            0.75f,
            Color(0.0f, 0.0f, 0.0f, 0.95f),
            { 0.0f, 0.0f }
        };

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

        for (int i = 0; i < LevelManager::TOTAL_LEVEL_COUNT; ++i) {
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