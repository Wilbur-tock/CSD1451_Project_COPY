/*
Authors: Bryan, Dayrius
Description: Display the level selection page

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "LevelSelect.hpp"

#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "CameraManager.hpp"
#include "AudioManager.hpp"
#include "Color.hpp"
#include "Utils.hpp"
#include "Config.hpp"
#include "UICursor.hpp"
#include "LevelManager.hpp"
#include "BgAnimation.hpp"
#include "ParticlesManager.hpp"
#include "UIManager.hpp"

namespace LevelSelect {

    namespace {
        constexpr int kTotalButtons = Config::TOTAL_LEVEL_COUNT;

        std::vector<Button> levelButtons;

        bool ignoredTutorialLevel = false;
        UIManager::PanelText titlePanel;
        Button backButton;
        Button resetProgressButton;
        TextObject hintText;

        AEGfxTexture* bgTexture = nullptr;
        AEGfxTexture* rectButtonTexture = nullptr;
        AEGfxTexture* squareButtonTexture = nullptr;

        constexpr float kLevelButtonSize = 170.0f;
        constexpr float kLevelButtonGap = 50.0f;
        constexpr float kLevelButtonRowCenterY = 40.0f;
        constexpr int kLastSelectableLevelHotkey = 3;

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
                static_cast<int>(GameStateManager::GameState::LEVEL_0) + levelIndex
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

            std::string label = levelIndex == 0 ? "Sandbox" : std::to_string(levelIndex);

            Color buttonColor(0.46f, 0.46f, 0.50f, 1.0f);
            Color textColor(0.20f, 0.20f, 0.22f, 1.0f);

            if (unlocked) {
                buttonColor = Color(0.88f, 0.92f, 0.62f, 1.0f);
                textColor = Color(0.16f, 0.16f, 0.16f, 1.0f);
            }

            button = Button(
                TextObject{ label, 1.0f, textColor },
                Utils::AnchorMode::CENTER,
                topLeftPos,
                { buttonSize, buttonSize },
                buttonColor,
                squareButtonTexture
            );
        }

        void RefreshLevelButtons() {
            levelButtons.clear();

            for (int i = 0; i < kTotalButtons; ++i) {
                levelButtons.push_back(Button{});

                int levelId = i;
                AEVec2 buttonTopLeft = GetLevelButtonTopLeft(
                    i,
                    kTotalButtons,
                    kLevelButtonSize,
                    kLevelButtonGap,
                    kLevelButtonRowCenterY
                );

                ConfigureLevelButton(levelButtons[i], i, buttonTopLeft, kLevelButtonSize);

                levelButtons[i].SetOnClick([levelId]() {
                    if (LevelManager::IsLevelUnlocked(levelId)) {
                        AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                        OpenLevel(levelId);
                    }
                    });
            }
        }
    }

    void Load() {
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");
        rectButtonTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");
        squareButtonTexture = AssetsManager::GetTexture("Assets/buttonTexture2.png");

        // Load shared animated background assets (clouds + cannonball projectiles)
        BgAnimation::Load();
    }

    void Initialize() {
        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        // Ensure Level Select starts fresh each time it is entered
        levelButtons.clear();

        // Reset and stagger the animated background (clouds + projectiles)
        BgAnimation::Init();

        RefreshLevelButtons();

        backButton = Button(
            TextObject{ "BACK", 0.85f, Color{ 1,1,1 } },
            Utils::AnchorMode::CENTER,
            { -150.0f, -250.0f },
            { 300.0f, 75.0f },
            Color(0.70f, 0.22f, 0.18f, 1.0f),
            rectButtonTexture,
            nullptr,
            []() {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnBackClicked();
            }
        );

        resetProgressButton = Button(
            TextObject{ "RESET PROGRESS", 0.85f, Color{ 1,1,1 } },
            Utils::AnchorMode::CENTER,
            { 60.0f, -250.0f },
            { 300.0f, 75.0f },
            Color(0.90f, 0.58f, 0.18f, 1.0f),
            rectButtonTexture,
            nullptr,
            []() {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                LevelManager::ResetProgress();
                LevelManager::SaveProgress();
                RefreshLevelButtons();
            }
        );

        hintText = {
            "Grey buttons are locked   |   0 = Sandbox, 1 = Level 1, 2 = Level 2, 3 = Level 3   |   RESET PROGRESS resets campaign unlocks",
            0.60f,
            Color(0.0f, 0.0f, 0.0f, 0.95f),
            { -0.95f, -0.92f }
        };

        titlePanel = UIManager::PanelText{
            UIManager::Panel{{250,50}, Color{0,0,0, 0.5f}},
            TextObject{"SELECT LEVEL", 1.0f, Color{1,1,1}, {}, Utils::AnchorMode::CENTER}
        };

        titlePanel.AddOffsetPosition({ 0,100 });
    }

    void Draw() {
        // Sky base drawn first so clouds and projectiles sit on top of it
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

        // Animated background: scrolling clouds + arcing cannonball projectiles
        BgAnimation::Draw();
        titlePanel.Draw();
        hintText.Draw();
    }

    void Update() {
        float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());

        // Advance cloud scroll and projectile physics for this frame
        BgAnimation::Update(dt);

        // Debug Inputs
        if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_BACK)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnBackClicked();
            return;
        }

        if (AEInputCheckTriggered(AEVK_0) && LevelManager::IsLevelUnlocked(0)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OpenLevel(0);
            return;
        }

        if (AEInputCheckTriggered(AEVK_1) && LevelManager::IsLevelUnlocked(1)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OpenLevel(1);
            return;
        }

        if (AEInputCheckTriggered(AEVK_2) && LevelManager::IsLevelUnlocked(2)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OpenLevel(2);
            return;
        }

        if (AEInputCheckTriggered(AEVK_3) && LevelManager::IsLevelUnlocked(3)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OpenLevel(3);
            return;
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
        //resetProgressButton.Update();
    }

    void Free() {
        levelButtons.clear();
    }

    void Unload() {}

    void OnBackClicked() {
        GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
    }
}