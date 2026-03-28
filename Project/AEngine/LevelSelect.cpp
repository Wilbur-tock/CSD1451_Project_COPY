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
                AssetsManager::squareTexture
            );
        }
    }

    void Load() {
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");
        buttonTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");

        // Load shared animated background assets (clouds + cannonball projectiles)
        BgAnimation::Load();
    }

    void Initialize() {
        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        // Reset and stagger the animated background (clouds + projectiles)
        BgAnimation::Init();

        for (int i = 0; i < kTotalButtons; ++i) {
            if (ignoredTutorialLevel) continue;

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

            // Added on blick function
            levelButtons[i].SetOnClick([levelId]() {
                    if (LevelManager::IsLevelUnlocked(levelId)) {
                        OpenLevel(levelId);
                    }
                }
            );
        }

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
        const std::string progressLabel =
            "Currently unlocked: LEVEL " + std::to_string(LevelManager::HighestUnlockedLevel() + 1);

        titlePanel.Draw();

        hintText.Draw();
    }

    void Update() {
        float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());

        // Advance cloud scroll and projectile physics for this frame
        BgAnimation::Update(dt);

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

    void OnBackClicked() {
        GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
    }
}