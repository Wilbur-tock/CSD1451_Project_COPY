#include "LevelSelect.hpp"

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
        std::vector<Button> levelButtons;
        Button backButton;

        TextObject titleText;
        TextObject subtitleText;
        TextObject hintText;

        AEGfxTexture* bgTexture = nullptr;
        AEGfxTexture* buttonTexture = nullptr;

        void OnBackClicked() {
            GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
        }

        void OnDummyLevelButtonClicked() {
            // Empty on purpose.
            // The button still records IsClick(), and Update() handles the index.
        }

        void StartLevel(int levelIndex) {
            LevelManager::SelectLevel(levelIndex);

            if (LevelManager::CurrentLevel() != nullptr) {
                GameStateManager::next = GameStateManager::GameState::LEVEL;
            }
        }

        Color GetLevelButtonColor(int index) {
            switch (index % 5) {
            case 0: return Color(0.25f, 0.70f, 0.35f, 1.0f);
            case 1: return Color(0.25f, 0.45f, 0.90f, 1.0f);
            case 2: return Color(0.85f, 0.60f, 0.20f, 1.0f);
            case 3: return Color(0.65f, 0.30f, 0.85f, 1.0f);
            default: return Color(0.85f, 0.35f, 0.35f, 1.0f);
            }
        }

        std::string BuildHintText(int levelCount) {
            if (levelCount <= 0) {
                return "No playable levels found. Add files like Assets/DataMap/level1.txt";
            }

            int quickSelectCount = (levelCount < 9) ? levelCount : 9;

            std::string hint = "Mouse: Select a level";
            if (quickSelectCount > 0) {
                hint += "   |   [1-" + std::to_string(quickSelectCount) + "] Quick Select";
                if (levelCount > 9) {
                    hint += " (first 9 only)";
                }
            }

            hint += "   |   ESC / BACK - Main Menu";
            return hint;
        }
    }

    void Load() {
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");
        buttonTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");
    }

    void Initialize() {
        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        levelButtons.clear();

        const int levelCount = LevelManager::LevelCount();

        const float btnWidth = 320.0f;
        const float btnHeight = 90.0f;
        const float centerX = 0.0f;
        const float spacing = 120.0f;

        float startY = (static_cast<float>(levelCount) * spacing) * 0.5f;
        if (levelCount == 0) {
            startY = 60.0f;
        }

        levelButtons.reserve(levelCount);

        for (int i = 0; i < levelCount; ++i) {
            Button levelButton(
                TextObject(LevelManager::GetLevelName(i)),
                Utils::AnchorMode::CENTER,
                AEVec2{ centerX - btnWidth / 2.0f, startY - (static_cast<float>(i) * spacing) },
                AEVec2{ btnWidth, btnHeight },
                GetLevelButtonColor(i),
                buttonTexture,
                nullptr,
                OnDummyLevelButtonClicked
            );

            levelButtons.push_back(levelButton);
        }

        backButton = Button(
            TextObject("< BACK"),
            Utils::AnchorMode::CENTER,
            AEVec2{ centerX - btnWidth / 2.0f, startY - (static_cast<float>(levelCount) * spacing) },
            AEVec2{ btnWidth, btnHeight },
            Color(0.85f, 0.25f, 0.25f, 1.0f),
            buttonTexture,
            nullptr,
            OnBackClicked
        );

        titleText = TextObject(
            "SELECT LEVEL",
            2.5f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            AEVec2{ -0.22f, 0.72f }
        );

        std::string subtitle =
            (levelCount > 0)
            ? "Choose which battlefield you want to play"
            : "No level files matching level<number>.txt were found";

        subtitleText = TextObject(
            subtitle,
            0.9f,
            Color(0.0f, 0.0f, 0.0f, 0.9f),
            AEVec2{ -0.42f, 0.57f }
        );

        hintText = TextObject(
            BuildHintText(levelCount),
            0.7f,
            Color(0.0f, 0.0f, 0.0f, 0.95f),
            AEVec2{ -0.92f, -0.92f }
        );
    }

    void Draw() {
        if (bgTexture != nullptr) {
            GraphicsManager::DrawWorldUI(
                bgTexture,
                GraphicsManager::CreateTransformMatrix(
                    -Config::SCREEN_WIDTH / 2.0f,
                    Config::SCREEN_HEIGHT / 2.0f,
                    0,
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
        hintText.Draw();
    }

    void Update() {
        if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_BACK)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnBackClicked();
            return;
        }

        static const unsigned int levelKeys[] = {
            AEVK_1, AEVK_2, AEVK_3, AEVK_4, AEVK_5,
            AEVK_6, AEVK_7, AEVK_8, AEVK_9
        };

        int quickSelectCount = LevelManager::LevelCount();
        if (quickSelectCount > 9) {
            quickSelectCount = 9;
        }

        for (int i = 0; i < quickSelectCount; ++i) {
            if (AEInputCheckTriggered(levelKeys[i])) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                StartLevel(i);
                return;
            }
        }

        Draw();

        for (int i = 0; i < static_cast<int>(levelButtons.size()); ++i) {
            levelButtons[i].Update();

            if (levelButtons[i].IsClick()) {
                StartLevel(i);
                return;
            }
        }

        backButton.Update();
        UICursor::Draw();
    }

    void Free() {}

    void Unload() {}
}