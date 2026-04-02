/*
Authors: Nixon
Description: Display and manage functions about the pause menu page

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "PauseMenu.hpp"
#include "ConfirmDialog.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"
#include "UIManager.hpp"
#include "Utils.hpp"
#include "Color.hpp"
#include "Config.hpp"

namespace PauseMenu {

    bool isInitialized = false; // Set true at the end of Init() to prevent rebuilding buttons more than once in a session

    // Buttons
    static Button resumeButton;
    static Button restartButton;   // new -- restart with confirmation
    static Button settingsButton;
    static Button backToMenuButton;

    // Text
    static TextObject titleText;
    static TextObject hintText;

    // Cached texture
    static AEGfxTexture* btnTexture = nullptr;

    // -----------------------------------------------------------------------
    void Init() {
        if (isInitialized) return; // If Init() has already run, do nothing and return immediately.
        // Without this, every call to Init() would reconstruct all buttons
        // and reset their state.

        btnTexture = AssetsManager::GetTexture("Assets/buttonTexture.png"); // Fetch from cache
        // (loaded by MainMenu::Load)

// Button dimensions
        float btnW = 300.0f; // Width of every button in world units
        float btnH = 70.0f;  // Height of every button in world units
        float spacing = 90.0f;  // Vertical distance between button top-left corners
        float centerX = 0.0f;   // Horizontal center in AE world space
        float startY = 105.0f; // Y of the topmost button top-left (raised to fit 4 buttons)

        // Resume Button -- non-destructive; no confirmation needed
        AEVec2 resumePos = { centerX - btnW / 2.0f, startY };
        resumeButton = Button(
            TextObject{ "RESUME" },
            Utils::AnchorMode::CENTER,
            resumePos,
            { btnW, btnH },
            Color(0.2f, 0.7f, 0.3f, 1.0f),  // green
            btnTexture,
            nullptr,
            OnResumeClicked
        );

        // Restart Button -- destructive: progress in the current level is lost
        AEVec2 restartPos = { centerX - btnW / 2.0f, startY - spacing };
        restartButton = Button(
            TextObject{ "RESTART" },
            Utils::AnchorMode::CENTER,
            restartPos,
            { btnW, btnH },
            Color(0.9f, 0.55f, 0.2f, 1.0f), // orange -- signals caution
            btnTexture,
            nullptr,
            OnRestartClicked
        );

        // Settings Button -- non-destructive
        AEVec2 settingsPos = { centerX - btnW / 2.0f, startY - spacing * 2 };
        settingsButton = Button(
            TextObject{ "SETTINGS" },
            Utils::AnchorMode::CENTER,
            settingsPos,
            { btnW, btnH },
            Color(0.6f, 0.6f, 0.2f, 1.0f),  // yellow
            btnTexture,
            nullptr,
            OnSettingsClicked
        );

        // Back to Menu Button -- destructive: current level progress is lost
        AEVec2 menuPos = { centerX - btnW / 2.0f, startY - spacing * 3 };
        backToMenuButton = Button(
            TextObject{ "BACK TO MENU" },
            Utils::AnchorMode::CENTER,
            menuPos,
            { btnW, btnH },
            Color(0.8f, 0.2f, 0.2f, 1.0f),  // red
            btnTexture,
            nullptr,
            OnBackToMenuClicked
        );

        // Title
        titleText = {
            "PAUSED",
            2.5f,
            Color(1.0f, 1.0f, 1.0f, 1.0f),
            { -0.13f, 0.65f } // shifted up slightly to accommodate 4 buttons
        };

        // Hint
        hintText = {
            "Press [ESC] to resume",
            0.7f,
            Color(1.0f, 1.0f, 1.0f, 0.6f),
            { -0.10f, -0.65f }
        };

        isInitialized = true; // Mark initialisation complete
    }

    // -----------------------------------------------------------------------
    void Draw() {
        // Full-screen dim overlay
        UIManager::Panel dim{
            { static_cast<float>(Config::SCREEN_WIDTH),
              static_cast<float>(Config::SCREEN_HEIGHT) },
            Color(0.0f, 0.0f, 0.0f, 0.6f), // 60% opacity
            Utils::AnchorMode::CENTER
        };
        dim.Draw();

        // Center panel -- slightly taller to fit the extra Restart button
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

    // -----------------------------------------------------------------------
    void Update() {
        // ESC key toggling is handled entirely by GameStateManager, not here.
        // If ESC were also checked here, pressing ESC would toggle the paused
        // state twice (caused a double-toggle bug in earlier iterations).

        // Draw pause overlay first, then UI on top of it.
        Draw();

        // -----------------------------------------------------------------------
        // Confirm dialog intercept
        //
        // When a destructive action has been triggered (Restart or Back to Menu),
        // ConfirmDialog::Open() was called by the callback. On subsequent frames,
        // ConfirmDialog::IsOpen() is true, so we draw the dialog on top of the
        // pause menu and skip the pause-menu button updates entirely.
        //
        // This prevents clicks from landing on pause buttons while the dialog is
        // covering them -- without this guard the YES/NO click could simultaneously
        // activate a button in the layer below.
        // -----------------------------------------------------------------------
        if (ConfirmDialog::IsOpen()) {
            ConfirmDialog::Update();
            return; // skip regular button updates while confirm is visible
        }

        // Normal button updates (only reached when no confirm dialog is open)
        resumeButton.Update();
        restartButton.Update();
        settingsButton.Update();
        backToMenuButton.Update();
    }

    // -----------------------------------------------------------------------
    // Callbacks
    // -----------------------------------------------------------------------

    void OnResumeClicked() {
        Utils::isPaused = false;        // Clear the global pause flag so the game loop
        // stops calling PauseMenu::Update() next frame
        Utils::unpauseCooldown = 0.15f; // Brief delay so the click does not also fire a bullet
    }

    void OnRestartClicked() {
        // Restarting loses all progress in the current level -- show confirmation.
        ConfirmDialog::Open(
            "All current level progress will be lost.",
            "Are you sure you want to restart?",
            []() {
                // Confirmed: clear pause state then restart.
                // Clearing isPaused first ensures the game loop does not try to keep
                // rendering the pause menu during the transition frame.
                Utils::isPaused = false;
                GameStateManager::next = GameStateManager::GameState::RESTART;
            }
        );
    }

    void OnSettingsClicked() {
        Utils::isSettingsOpen = true; // Show settings overlay on top of pause menu
    }

    void OnBackToMenuClicked() {
        // Returning to the main menu loses all progress -- show confirmation.
        ConfirmDialog::Open(
            "All current level progress will be lost.",
            "Are you sure you want to return to the main menu?",
            []() {
                // Confirmed: clear pause state then transition.
                Utils::isPaused = false;
                GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
            }
        );
    }

} // namespace PauseMenu