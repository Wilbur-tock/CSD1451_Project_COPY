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
        constexpr int kTotalButtons = 5;

        std::array<Button, kTotalButtons> levelButtons;
        Button backButton;

        TextObject titleText;
        TextObject subtitleText;
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
            std::string label = "LEVEL " + std::to_string(levelIndex + 1);

            Color buttonColor = Color(0.30f, 0.30f, 0.30f, 0.95f);
            if (unlocked) {
                buttonColor = Color(0.25f, 0.55f + (0.06f * levelIndex), 0.40f, 1.0f);
            }
            else {
                label += "  [LOCKED]";
            }

            button = Button(
                TextObject{ label },
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

    void Load() {
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");
        buttonTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");
    }

    void Initialize() {
        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        const float btnWidth = 340.0f;
        const float btnHeight = 75.0f;
        const float centerX = -btnWidth / 2.0f;
        const float startY = 220.0f;
        const float spacing = 95.0f;

        ConfigureLevelButton(levelButtons[0], 0, centerX, startY, btnWidth, btnHeight);
        levelButtons[0].SetOnClick(OnLevel1Clicked);

        ConfigureLevelButton(levelButtons[1], 1, centerX, startY - spacing, btnWidth, btnHeight);
        levelButtons[1].SetOnClick(LevelManager::IsLevelUnlocked(1) ? OnLevel2Clicked : nullptr);

        ConfigureLevelButton(levelButtons[2], 2, centerX, startY - spacing * 2.0f, btnWidth, btnHeight);
        levelButtons[2].SetOnClick(LevelManager::IsLevelUnlocked(2) ? OnLevel3Clicked : nullptr);

        ConfigureLevelButton(levelButtons[3], 3, centerX, startY - spacing * 3.0f, btnWidth, btnHeight);
        levelButtons[3].SetOnClick(LevelManager::IsLevelUnlocked(3) ? OnLevel4Clicked : nullptr);

        ConfigureLevelButton(levelButtons[4], 4, centerX, startY - spacing * 4.0f, btnWidth, btnHeight);
        levelButtons[4].SetOnClick(LevelManager::IsLevelUnlocked(4) ? OnLevel5Clicked : nullptr);

        backButton = Button(
            TextObject{ "< BACK" },
            Utils::AnchorMode::CENTER,
            { centerX, startY - spacing * 5.35f },
            { btnWidth, btnHeight },
            Color(0.85f, 0.25f, 0.25f, 1.0f),
            buttonTexture,
            nullptr,
            OnBackClicked
        );

        titleText = {
            "SELECT LEVEL",
            1.5f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { -0.22f, 0.82f }
        };

        subtitleText = {
            "Beat a level to unlock the next one",
            0.9f,
            Color(0.0f, 0.0f, 0.0f, 0.9f),
            { -0.25f, 0.68f }
        };

        hintText = {
            "1-5 = Select unlocked level   |   ESC / BACK = Main Menu",
            0.7f,
            Color(0.0f, 0.0f, 0.0f, 0.95f),
            { -0.80f, -0.94f }
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

        titleText.Draw();
        subtitleText.Draw();

        TextObject progressText{
            "Highest unlocked: LEVEL " + std::to_string(LevelManager::HighestUnlockedLevel() + 1),
            0.8f,
            Color(0.0f, 0.0f, 0.0f, 0.95f),
            { -0.18f, 0.60f }
        };
        progressText.Draw();

        hintText.Draw();
    }

    void Update() {
        // Check for inputs

        if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_BACK)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnBackClicked();
            return;
        }

        // Debug inputs
        for (int i = 0; i < LevelManager::TOTAL_LEVEL_COUNT; ++i) {
            if (AEInputCheckTriggered(static_cast<u8>(AEVK_1 + i)) && LevelManager::IsLevelUnlocked(i)) {
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
    void OnLevel4Clicked() { OpenLevel(3); }
    void OnLevel5Clicked() { OpenLevel(4); }

    void OnBackClicked() {
        GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
    }
}