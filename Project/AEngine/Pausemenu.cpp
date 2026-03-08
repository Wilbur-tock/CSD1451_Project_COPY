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

    bool isInitialized = false; // Set true at the end of Init() to prevent rebuilding buttons more than once in a session

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
        if (isInitialized) return; // if Init() has already run, do nothing and return immediately.
                                   // Without this, every call to Init() would reconstruct all buttons and reset their state

        btnTexture = AssetsManager::GetTexture("Assets/buttonTexture.png"); // Fetch the button texture from the cache 
                                                                            // (Loaded from Main Menu)

        // Button dimensions
        float btnW = 300.0f; // Width of all three buttons in world units
        float btnH = 70.0f; // Height of all three buttons in world units
        float spacing = 90.0f; // Vertical distance between the top-left corners of consecutive buttons
        float centerX = 0.0f; // X origin of the button column. 0 = horizontal center of the screen in AE world space
        float startY = 60.0f; // Y position of the topmost button (Resume)

        // Resume Button
        AEVec2 resumePos = { centerX - btnW / 2.0f, startY }; // Shift left by half the button width so the button visually centers on centerX.
        resumeButton = Button(
            TextObject{ "RESUME" },
            Utils::AnchorMode::CENTER,
            resumePos,
            { btnW, btnH },
            Color(0.2f, 0.7f, 0.3f, 1.0f),   // green
            btnTexture,
            OnResumeClicked
        );

        // Settings Button
        AEVec2 settingsPos = { centerX - btnW / 2.0f, startY - spacing }; // One spacing unit below Resume
        settingsButton = Button(
            TextObject{ "SETTINGS" },
            Utils::AnchorMode::CENTER,
            settingsPos,
            { btnW, btnH },
            Color(0.6f, 0.6f, 0.2f, 1.0f),   // yellow
            btnTexture,
            OnSettingsClicked
        );

        // Back To Menu Button
        AEVec2 menuPos = { centerX - btnW / 2.0f, startY - spacing * 2 }; // Two spacing unit below Resume
        backToMenuButton = Button(
            TextObject{ "BACK TO MENU" },
            Utils::AnchorMode::CENTER,
            menuPos,
            { btnW, btnH },
            Color(0.8f, 0.2f, 0.2f, 1.0f),   // red
            btnTexture,
            OnBackToMenuClicked
        );

        // Title
        titleText = {
            "PAUSED",
            2.5f,
            Color(1.0f, 1.0f, 1.0f, 1.0f),
            { -0.13f, 0.55f }
        };

        // Hint
        hintText = {
            "Press [ESC] to resume",
            0.7f,
            Color(1.0f, 1.0f, 1.0f, 0.6f),
            {-0.13f, -0.55f }
        };

        isInitialized = true; // Mark initialisation complete
    }

    void Draw() {
        // Full-screen dim overlay
        UIManager::Panel dim{
            { static_cast<float>(Config::SCREEN_WIDTH),
              static_cast<float>(Config::SCREEN_HEIGHT) },
            Color(0.0f, 0.0f, 0.0f, 0.6f), // 60% Opacity
            Utils::AnchorMode::CENTER // The panel is centered on the world origin (0,0)
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

    void Update() {
        // ESC key toggling is handled entirely by GameStateManager, not here.
        // If ESC were also checked here, pressing ESC would toggle the paused state twice (had a bug here)
        
        // Draw overlay first, then buttons on top
        Draw();

        // Buttons are updated after Draw() so they render on top of the panel and dim overlay
        resumeButton.Update();
        settingsButton.Update();
        backToMenuButton.Update();
    }

    // Callbacks
    void OnResumeClicked() {
        Utils::isPaused = false;         // Clear the global pause flag 
                                         // (used to prevent settings opening back to main menu instead of pause menu)
        Utils::unpauseCooldown = 0.15f;  // Brief delay so the click does not fire a bullet
    }

    void OnSettingsClicked() {
        Utils::isSettingsOpen = true;    // Show settings on top of pause
    }

    void OnBackToMenuClicked() {
        Utils::isPaused = false; // Clear the pause flag first so the game loop does not try to
                                 // keep rendering the pause menu during the transition frame
        GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
    }
}