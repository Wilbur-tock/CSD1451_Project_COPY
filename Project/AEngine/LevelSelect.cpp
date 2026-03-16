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

namespace LevelSelect {

    static Button level1Button;
    static Button level2Button;
    static Button backButton;

    static TextObject titleText;
    static TextObject subtitleText;
    static TextObject hintText;

    static AEGfxTexture* bgTexture = nullptr;
    static AEGfxTexture* buttonTexture = nullptr;

    void Load() {
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");
        buttonTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");
    }

    void Initialize() {
        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        const float btnWidth = 320.0f;
        const float btnHeight = 90.0f;
        const float centerX = 0.0f;
        const float startY = 80.0f;
        const float spacing = 120.0f;

        level1Button = Button(
            TextObject{ "LEVEL 1" },
            Utils::AnchorMode::CENTER,
            { centerX - btnWidth / 2.0f, startY },
            { btnWidth, btnHeight },
            Color(0.25f, 0.7f, 0.35f, 1.0f),
            buttonTexture,
            nullptr,
            OnLevel1Clicked
        );

        level2Button = Button(
            TextObject{ "LEVEL 2" },
            Utils::AnchorMode::CENTER,
            { centerX - btnWidth / 2.0f, startY - spacing },
            { btnWidth, btnHeight },
            Color(0.25f, 0.45f, 0.9f, 1.0f),
            buttonTexture,
            nullptr,
            OnLevel2Clicked
        );

        backButton = Button(
            TextObject{ "< BACK" },
            Utils::AnchorMode::CENTER,
            { centerX - btnWidth / 2.0f, startY - spacing * 2.0f },
            { btnWidth, btnHeight },
            Color(0.85f, 0.25f, 0.25f, 1.0f),
            buttonTexture,
            nullptr,
            OnBackClicked
        );

        titleText = {
            "SELECT LEVEL",
            2.5f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { -0.22f, 0.72f }
        };

        subtitleText = {
            "Choose which battlefield you want to play",
            0.9f,
            Color(0.0f, 0.0f, 0.0f, 0.9f),
            { -0.34f, 0.57f }
        };

        hintText = {
            "1 - Level 1   |   2 - Level 2   |   ESC / BACK - Main Menu",
            0.7f,
            Color(0.0f, 0.0f, 0.0f, 0.95f),
            { -0.78f, -0.92f }
        };
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

        if (AEInputCheckTriggered(AEVK_1)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnLevel1Clicked();
            return;
        }

        if (AEInputCheckTriggered(AEVK_2)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnLevel2Clicked();
            return;
        }

        Draw();

        level1Button.Update();
        level2Button.Update();
        backButton.Update();

        UICursor::Draw();
    }

    void Free() {}

    void Unload() {}

    void OnLevel1Clicked() {
        GameStateManager::next = GameStateManager::GameState::LEVEL_1;
    }

    void OnLevel2Clicked() {
        GameStateManager::next = GameStateManager::GameState::LEVEL_2;
    }

    void OnBackClicked() {
        GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
    }
}