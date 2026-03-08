#include "PauseMenu.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"
#include "UIManager.hpp"
#include "Utils.hpp"
#include "Color.hpp"
#include "Config.hpp"

namespace PauseMenu {

    bool isInitialized = false;

    // Buttons
    static Button resumeButton;
    static Button settingsButton;
    static Button backToMenuButton;

    // Text
    static TextObject titleText;
    static TextObject hintText;

    // Cached texture
    static AEGfxTexture* btnTexture = nullptr;

    // ---------------------------------------------------------------
    void Init() {
        if (isInitialized) return;

        btnTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");

        float btnW = 300.0f;
        float btnH = 70.0f;
        float spacing = 90.0f;
        float centerX = 0.0f;
        float startY = 60.0f;

        // --- Resume Button ---
        AEVec2 resumePos = { centerX - btnW / 2.0f, startY };
        resumeButton = Button(
            TextObject{ "RESUME" },
            Utils::AnchorMode::CENTER,
            resumePos,
            { btnW, btnH },
            Color(0.2f, 0.7f, 0.3f, 1.0f),   // green
            btnTexture,
            OnResumeClicked
        );

        // --- Settings Button ---
        AEVec2 settingsPos = { centerX - btnW / 2.0f, startY - spacing };
        settingsButton = Button(
            TextObject{ "SETTINGS" },
            Utils::AnchorMode::CENTER,
            settingsPos,
            { btnW, btnH },
            Color(0.6f, 0.6f, 0.2f, 1.0f),   // yellow
            btnTexture,
            OnSettingsClicked
        );

        // --- Back To Menu Button ---
        AEVec2 menuPos = { centerX - btnW / 2.0f, startY - spacing * 2 };
        backToMenuButton = Button(
            TextObject{ "BACK TO MENU" },
            Utils::AnchorMode::CENTER,
            menuPos,
            { btnW, btnH },
            Color(0.8f, 0.2f, 0.2f, 1.0f),   // red
            btnTexture,
            OnBackToMenuClicked
        );

        // --- Title ---
        titleText = {
            "PAUSED",
            2.5f,
            Color(1.0f, 1.0f, 1.0f, 1.0f),
            { -0.13f, 0.55f }
        };

        // --- Hint ---
        hintText = {
            "Press [ESC] to resume",
            0.7f,
            Color(1.0f, 1.0f, 1.0f, 0.6f),
            {-0.13f, -0.55f }
        };

        isInitialized = true;
    }

    // ---------------------------------------------------------------
    void Draw() {
        // Full-screen dim overlay
        UIManager::Panel dim{
            { static_cast<float>(Config::SCREEN_WIDTH),
              static_cast<float>(Config::SCREEN_HEIGHT) },
            Color(0.0f, 0.0f, 0.0f, 0.6f),
            Utils::AnchorMode::CENTER
        };
        dim.Draw();

        // Center panel
        UIManager::Panel panel{
            { 500.0f, 800.0f },
            Color(0.1f, 0.1f, 0.14f, 0.92f),
            Utils::AnchorMode::CENTER
        };
        panel.Draw();

        // Title and hint
        titleText.Draw();
        hintText.Draw();
    }

    // ---------------------------------------------------------------
    void Update() {
        // NOTE: ESC toggling is handled entirely by GameStateManager.
        // Do NOT check ESC here -- it would double-toggle on the same
        // frame and the menu would open and close instantly.

        // Draw overlay first, then buttons on top
        Draw();

        resumeButton.Update();
        settingsButton.Update();
        backToMenuButton.Update();
    }

    // ---------------------------------------------------------------
    // Callbacks
    // ---------------------------------------------------------------
    void OnResumeClicked() {
        Utils::isPaused = false;
        Utils::unpauseCooldown = 0.15f;  // brief delay so the click does not fire a bullet
    }

    void OnSettingsClicked() {
        Utils::isPaused = false;
        Utils::returnToPause = true;                               // <-- NEW: remember we came from pause
        GameStateManager::next = GameStateManager::GameState::SETTINGS;
    }

    void OnBackToMenuClicked() {
        Utils::isPaused = false;
        GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
    }
}