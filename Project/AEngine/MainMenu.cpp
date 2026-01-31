#include "MainMenu.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"
#include "Utils.hpp"

namespace MainMenu {
    // Menu buttons
    Button playButton;
    Button mapEditorButton;
    Button creditsButton;
    Button quitButton;

    // Menu state
    bool isInitialized = false;

    // Menu title
    std::string title = "Justin Likes Kids";

    // Background texture
    AEGfxTexture* bgTexture = nullptr;
    AEGfxTexture* buttonTexture = nullptr;

    void Load() {
        // Load any textures or assets needed for the menu
        // This runs once when the game starts
        buttonTexture = AssetsManager::GetTexture("Assets/square.png");
        bgTexture = AssetsManager::GetTexture("Assets/Harambe.png");
        bgTexture = nullptr;
    }

    void Initialize() {
        // Initialize menu elements
        isInitialized = true;

        // Button dimensions
        float btnWidth = 300.0f;
        float btnHeight = 80.0f;
        float btnSpacing = 100.0f;

        // Calculate centered positions
        float centerX = 0.0f;  // Center of screen in world coordinates
        float startY = 100.0f; // Starting Y position for first button

        // Create Play Button
        AEVec2 playPos = { centerX - btnWidth / 2.0f, startY };
        playButton = Button(
            TextObject{ "PLAY" },
            Utils::RECT_CENTER,
            playPos,
            { btnWidth, btnHeight },
            Color(0.2f, 0.6f, 0.9f, 0.8f),  // Blue color
            buttonTexture,
            OnPlayClicked
        );

        // Create Map Editor Button
        AEVec2 editorPos = { centerX - btnWidth / 2.0f, startY - btnSpacing };
        mapEditorButton = Button(
            TextObject{ "MAP EDITOR" },
            Utils::RECT_CENTER,
            editorPos,
            { btnWidth, btnHeight },
            Color(0.3f, 0.7f, 0.3f, 0.8f),  // Green color
            buttonTexture,
            OnMapEditorClicked
        );

        // Create Credits Button
        AEVec2 creditsPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 2 };
        creditsButton = Button(
            TextObject{ "CREDITS" },
            Utils::RECT_CENTER,
            creditsPos,
            { btnWidth, btnHeight },
            Color(0.7f, 0.5f, 0.9f, 0.8f),  // Purple color
            buttonTexture,
            OnCreditsClicked
        );

        // Create Quit Button
        AEVec2 quitPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 3 };
        quitButton = Button(
            TextObject{ "QUIT" },
            Utils::RECT_CENTER,
            quitPos,
            { btnWidth, btnHeight },
            Color(0.8f, 0.2f, 0.2f, 0.8f),  // Red color
            buttonTexture,
            OnQuitClicked
        );
    }

    void Draw() {
        // Draw background image if available
        if (bgTexture != nullptr) {
            // Draw full-screen background
            GraphicsManager::DrawUI(
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
        GraphicsManager::DrawTextUI(
            TextObject{ title,
            { -0.3f, 0.65f }, // position (normalized)
            Color(1.0f, 1.0f, 1.0f, 1.0f),  // White color
            2.0f }    // Larger text scale for title
        );

        // Draw subtitle or instructions
        std::string instructions = "Click a button or use keyboard shortcuts:\n1 - Play Game | 2 - Map Editor | 3 - Credits | Q - Quit";
        GraphicsManager::DrawTextUI(
            TextObject{ instructions,
            { -0.98f, -0.9f },
            Color(0.7f, 0.7f, 0.7f, 1.0f),
            0.7f }  // Smaller text scale
        );

        // Draw buttons using GraphicsManager
        Color buttonTextColor(1.0f, 1.0f, 1.0f, 1.0f);  // White text
        GraphicsManager::DrawButton(playButton);
        GraphicsManager::DrawButton(mapEditorButton);
        GraphicsManager::DrawButton(creditsButton);
        GraphicsManager::DrawButton(quitButton);
    }

    void Update() {
        // Handle button interactions
        //bool playHovered = CollisionManager::IsPointInRect(
        //    playButton.pos.x, playButton.pos.y,
        //    playButton.width, playButton.height
        //);
        //bool editorHovered = CollisionManager::IsPointInRect(
        //    mapEditorButton.pos.x, mapEditorButton.pos.y,
        //    mapEditorButton.width, mapEditorButton.height
        //);
        //
        //bool creditsHovered = CollisionManager::IsPointInRect(
        //    creditsButton.pos.x, creditsButton.pos.y,
        //    creditsButton.width, creditsButton.height
        //);
        //
        //bool quitHovered = CollisionManager::IsPointInRect(
        //    quitButton.pos.x, quitButton.pos.y,
        //    quitButton.width, quitButton.height
        //);
        //
        //// Check for button clicks
        //if (AEInputCheckTriggered(AEVK_LBUTTON)) {
        //    if (playHovered) {
        //        OnPlayClicked();
        //    }
        //    else if (editorHovered) {
        //        OnMapEditorClicked();
        //    }
        //    else if (creditsHovered) {
        //        OnCreditsClicked();
        //    }
        //    else if (quitHovered) {
        //        OnQuitClicked();
        //    }
        //}

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
        if (AEInputCheckTriggered(AEVK_Q)) {
            OnQuitClicked();
        }

        Draw();
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
        GameStateManager::next = GameStateManager::GS_LEVEL_1;
    }

    void OnMapEditorClicked() {
        // Transition to Map Editor
        GameStateManager::next = GameStateManager::GS_MAP_EDITOR;
    }

    void OnCreditsClicked() {
        // Transition to Credits screen
        GameStateManager::next = GameStateManager::GS_CREDITS;
    }

    void OnQuitClicked() {
        // Quit the game
        GameStateManager::next = GameStateManager::GS_QUIT;
    }
}






