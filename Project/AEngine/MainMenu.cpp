#include "MainMenu.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"
#include "Utils.hpp"
#include "AudioManager.hpp"
#include "CameraManager.hpp"

namespace MainMenu {
    // Menu buttons
    Button playButton, mapEditorButton, creditsButton, settingsButton, guideButton, quitButton;
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
        float btnWidth = 300.0f;   // Width of every button in world units.
        float btnHeight = 80.0f;   // Height of every button in world units.
        float btnSpacing = 100.0f; // Vertical gap between button tops.

        // Calculate centered positions
        float centerX = -550.0f;  // Center of screen in world coordinates
        float startY = 200.0f; // Starting Y position for first button

        // Create Play Button
        // Transitions to LEVEL_1. Blue tint
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
        // Transitions to MAP_EDITOR. Green tint
        AEVec2 editorPos = { centerX - btnWidth / 2.0f, startY - btnSpacing }; // One spacing unit below the Play button.
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
        // Transitions to SETTINGS. Yellow tint
        AEVec2 settingsPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 2 }; // Two spacing units below Play.
        settingsButton = Button(
            TextObject{ "SETTINGS" },
            Utils::AnchorMode::CENTER,
            settingsPos,
            { btnWidth, btnHeight },
            Color(0.6f, 0.6f, 0.2f, 1.0f),  // Yellow color
            buttonTexture,
            OnSettingsClicked
        );

        // Create Guide Button
        // Transitions to GUIDE. Orange tint
        AEVec2 guidePos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 3 };
        guideButton = Button(
            TextObject{ "GUIDE" },
            Utils::AnchorMode::CENTER,
            guidePos,
            { btnWidth, btnHeight },
            Color(0.9f, 0.55f, 0.2f, 1.0f),
            buttonTexture,
            OnGuideClicked
        );

        // Create Credits Button
        // Transitions to CREDITS. Purple tint
        AEVec2 creditsPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 4 }; // Three spacing units below Play.
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
        // Sets GameState to QUIT. Red tint
        AEVec2 quitPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 5 }; // Four spacing units below Play; bottom of the column.
        quitButton = Button(
            TextObject{ "QUIT" },
            Utils::AnchorMode::CENTER,
            quitPos,
            { btnWidth, btnHeight },
            Color(0.8f, 0.2f, 0.2f, 1.0f),  // Red color
            buttonTexture,
            OnQuitClicked
        );

        // Game title text
        // Position is in normalized screen space (0 > 1 range mapped to screen)
        // Scale and color are re-written every frame by the animation in Update(),
        gameTitle = { "FORTRESS FIGHT",
            1.0f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),  // White color
            { 0.1f, 0.75f }, // position (normalized)
        };
        gameTitle.SetFont(AssetsManager::titleFont);

        // Keyboard shortcut hint
        // Rendered at the very bottom of the screen in small text
        instructions = {
        "Click a button or use keyboard shortcuts:\n1 - Play | 2 - Map Editor | 3 - Settings | 4 - Guide | 5 - Credits | Q - Quit",
        0.7f,
        Color(0.0f, 0.0f, 0.0f, 1.0f),
        { -0.98f, -0.9f },
        };

        // Reset camera to orgin pos and can't move
        CameraManager::camera.SetPosition({ 0, 0 }); // Move the camera to the world origin so UI world-space coordinates map directly to screen positions.
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC); // Prevent the camera from moving; keeps UI elements from drifting.
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
        gameTitle.TextScale(pulseScale);

        // Flash: alpha oscillates between 0.5 and 1.0
        // Avg. brightness + How much brightness vary * How fast brightness vary
        // `sinf` returns a value that cycles smoothly between `-1.0` and `+1.0`.
        float flashAlpha = 0.75f + 0.25f * sinf(titleAnimTimer * 3.0f);
        gameTitle.SetColor(Color{
            gameTitle.GetColor().Red(),
            gameTitle.GetColor().Green(),
            gameTitle.GetColor().Blue(),
            flashAlpha
        });

        // Keyboard shortcuts with sfx (on keyPressed, not held)
        if (AEInputCheckTriggered(AEVK_1)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnPlayClicked();
        }
        if (AEInputCheckTriggered(AEVK_2)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnMapEditorClicked();
        }
        if (AEInputCheckTriggered(AEVK_3)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnSettingsClicked();
        }
        if (AEInputCheckTriggered(AEVK_4)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnCreditsClicked();
        }
        if (AEInputCheckTriggered(AEVK_5)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnCreditsClicked();
        }
        if (AEInputCheckTriggered(AEVK_Q)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnQuitClicked();
        }

        Draw(); // Render background, title, and instructions for this frame before buttons draw on top.

        // Each button's Update() checks whether the mouse cursor is hovering over it,
        // Draws the button with the appropriate highlight state, and fires its callback if clicked.
        playButton.Update();
        mapEditorButton.Update();
        settingsButton.Update();
        guideButton.Update();
        creditsButton.Update();
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
    // Writing to GameStateManager::next does not switch states immediately.
    // The game loop reads next at the end of the frame and performs the transition then.
    void OnPlayClicked() {
        // Transition to Level Selector
        GameStateManager::next = GameStateManager::GameState::LEVEL_SELECT;
    }

    void OnMapEditorClicked() {
        // Transition to Map Editor
        GameStateManager::next = GameStateManager::GameState::MAP_EDITOR;
    }

    void OnSettingsClicked() {
        // Transition to Settings screen
        GameStateManager::next = GameStateManager::GameState::SETTINGS;
    }

    void OnCreditsClicked() {
        // Transition to Credits screen
        GameStateManager::next = GameStateManager::GameState::CREDITS;
    }

    void OnGuideClicked() {
        // Transition to Guide screen
        GameStateManager::next = GameStateManager::GameState::GUIDE;
    }

    void OnQuitClicked() {
        // Quit the game
        GameStateManager::next = GameStateManager::GameState::QUIT;
    }
}