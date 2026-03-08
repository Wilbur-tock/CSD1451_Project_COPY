#include "MainMenu.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"
#include "Utils.hpp"

namespace MainMenu {
    // Menu buttons
    Button playButton, mapEditorButton, creditsButton, settingsButton, quitButton;
    TextObject gameTitle, instructions;

    // Menu state
    bool isInitialized = false;

    // Background texture
    AEGfxTexture* bgTexture = nullptr;
    AEGfxTexture* buttonTexture = nullptr;

    // Game title animations initialize
    float titleAnimTimer = 0.0f;
    float baseTitleScale = 2.5f;

    void Load() {
        // Load any textures or assets needed for the menu
        // This runs once when the game starts
        buttonTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");
        /*bgTexture = nullptr;*/
    }

    void Initialize() {
        // Initialize menu elements
        isInitialized = true;

        // Button dimensions
        float btnWidth = 300.0f;
        float btnHeight = 80.0f;
        float btnSpacing = 100.0f;

        // Calculate centered positions
        float centerX = -550.0f;  // Center of screen in world coordinates
        float startY = 200.0f; // Starting Y position for first button

        // Create Play Button
        AEVec2 playPos = { centerX - btnWidth / 2.0f, startY };
        playButton = Button(
            TextObject{ "PLAY" },
            Utils::AnchorMode::CENTER,
            playPos,
            { btnWidth, btnHeight },
            Color(0.2f, 0.6f, 0.9f, 1.0f),  // Blue color
            buttonTexture,
            OnPlayClicked
        );

        // Create Map Editor Button
        AEVec2 editorPos = { centerX - btnWidth / 2.0f, startY - btnSpacing };
        mapEditorButton = Button(
            TextObject{ "MAP EDITOR" },
            Utils::AnchorMode::CENTER,
            editorPos,
            { btnWidth, btnHeight },
            Color(0.3f, 0.7f, 0.3f, 1.0f),  // Green color
            buttonTexture,
            OnMapEditorClicked
        );

        // Create Settings Button
        AEVec2 settingsPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 2 };
        settingsButton = Button(
            TextObject{ "SETTINGS" },
            Utils::AnchorMode::CENTER,
            settingsPos,
            { btnWidth, btnHeight },
            Color(0.6f, 0.6f, 0.2f, 1.0f),  // Yellow color
            buttonTexture,
            OnSettingsClicked
        );

        // Create Credits Button
        AEVec2 creditsPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 3 };
        creditsButton = Button(
            TextObject{ "CREDITS" },
            Utils::AnchorMode::CENTER,
            creditsPos,
            { btnWidth, btnHeight },
            Color(0.7f, 0.5f, 0.9f, 1.0f),  // Purple color
            buttonTexture,
            OnCreditsClicked
        );

        // Create Quit Button
        AEVec2 quitPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 4 };
        quitButton = Button(
            TextObject{ "QUIT" },
            Utils::AnchorMode::CENTER,
            quitPos,
            { btnWidth, btnHeight },
            Color(0.8f, 0.2f, 0.2f, 1.0f),  // Red color
            buttonTexture,
            OnQuitClicked
        );

        gameTitle = { "FORTRESS FIGHT",
            2.5f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),  // White color
            { 0.1f, 0.75f }, // position (normalized)
        };

        instructions = {
            "Click a button or use keyboard shortcuts:\n1 - Play Game | 2 - Map Editor | 3 - Credits | 4 - Settings | ESC - Quit",
            0.7f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { -0.98f, -0.9f },
        };

        // Reset camera to orgin pos and can't move
        CameraManager::camera.SetPosition({ 0, 0 });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);
    }

    void Draw() {
        // Draw background image if available
        if (bgTexture != nullptr) {
            // Draw full-screen background
            GraphicsManager::DrawWorldUI(
                bgTexture,
                GraphicsManager::CreateTransformMatrix(-Config::SCREEN_WIDTH / 2.0f, Config::SCREEN_HEIGHT / 2.0f, 0,
                    Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT),
                Color(1.0f, 1.0f, 1.0f, 1.0f)  // White = no color tinting
            );
        }
        else {
            // Fallback: solid background color if no texture
            AEGfxSetBackgroundColor(0.1f, 0.1f, 0.15f);  // Dark blue-gray background
        }

        // Draw title
        gameTitle.Draw();

        // Draw subtitle or instructions
        instructions.Draw();
    }

    void Update() {
        // Advance animation timer
        titleAnimTimer += static_cast<float>(AEFrameRateControllerGetFrameTime());

        // Pulse: scale oscillates +/- 0.2 around the base scale
        // How big the scale bounce * How fast it grows/shrinks (pulse)
        // `sinf` returns a value that cycles smoothly between `-1.0` and `+1.0`.
        float pulseScale = baseTitleScale + 0.2f * sinf(titleAnimTimer * 2.0f);
        gameTitle.textScale = pulseScale;

        // Flash: alpha oscillates between 0.5 and 1.0
        // Avg. brightness + How much brightness vary * How fast brightness vary
        // `sinf` returns a value that cycles smoothly between `-1.0` and `+1.0`.
        float flashAlpha = 0.75f + 0.25f * sinf(titleAnimTimer * 3.0f);
        gameTitle.color.SetColor(
            gameTitle.color.Red(),
            gameTitle.color.Green(),
            gameTitle.color.Blue(),
            flashAlpha
        );

        // Keyboard shortcuts
        if (AEInputCheckTriggered(AEVK_1)) {
            OnPlayClicked();
        }
        if (AEInputCheckTriggered(AEVK_2)) {
            OnMapEditorClicked();
        }
        if (AEInputCheckTriggered(AEVK_3)) {
            OnCreditsClicked();
        }
        if (AEInputCheckTriggered(AEVK_4)) {
            OnSettingsClicked();
        }
        if (AEInputCheckTriggered(AEVK_Q)) {
            OnQuitClicked();
        }

        Draw();

        playButton.Update();
        mapEditorButton.Update();
        creditsButton.Update();
        settingsButton.Update();
        quitButton.Update();
    }

    void Free() {
        // Clean up when leaving the main menu
        isInitialized = false;
    }

    void Unload() {
        // Unload any resources
        // Called when shutting down the game
    }

    // Button callback functions
    void OnPlayClicked() {
        // Transition to Level 1
        GameStateManager::next = GameStateManager::GameState::LEVEL_1;
    }

    void OnMapEditorClicked() {
        // Transition to Map Editor
        GameStateManager::next = GameStateManager::GameState::MAP_EDITOR;
    }

    void OnCreditsClicked() {
        // Transition to Credits screen
        GameStateManager::next = GameStateManager::GameState::CREDITS;
    }

    void OnSettingsClicked() {
        // Transition to Settings screen
        GameStateManager::next = GameStateManager::GameState::SETTINGS;
    }

    void OnQuitClicked() {
        // Quit the game
        GameStateManager::next = GameStateManager::GameState::QUIT;
    }
}