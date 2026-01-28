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
    Button quitButton;

    // Menu state
    bool isInitialized = false;

    // Menu title
    std::string title = "GAME TITLE";

    // Background texture
    AEGfxTexture* bgTexture = nullptr;
    AEGfxTexture* buttonTexture = nullptr;

    void Load() {
        // Load any textures or assets needed for the menu
        // This runs once when the game starts
        buttonTexture = AssetsManager::GetTexture("Assets/square.png");
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
        float startY = 200.0f; // Starting Y position for first button

        // Create Play Button
        AEVec2 playPos = { centerX - btnWidth / 2.0f, startY };
        playButton = Button(
            "PLAY GAME",
            playPos,
            btnWidth,
            btnHeight,
            Color(0.2f, 0.6f, 0.9f, 0.8f),  // Blue color
            buttonTexture,
            Utils::RECT_TOP_LEFT,
            OnPlayClicked
        );
        playButton.textOffsetX = 0.05f;
        playButton.textOffsetY = -0.02f;

        // Create Map Editor Button
        AEVec2 editorPos = { centerX - btnWidth / 2.0f, startY - btnSpacing };
        mapEditorButton = Button(
            "MAP EDITOR",
            editorPos,
            btnWidth,
            btnHeight,
            Color(0.3f, 0.7f, 0.3f, 0.8f),  // Green color
            buttonTexture,
            Utils::RECT_TOP_LEFT,
            OnMapEditorClicked
        );
        mapEditorButton.textOffsetX = 0.05f;
        mapEditorButton.textOffsetY = -0.02f;

        // Create Quit Button
        AEVec2 quitPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 2 };
        quitButton = Button(
            "QUIT",
            quitPos,
            btnWidth,
            btnHeight,
            Color(0.8f, 0.2f, 0.2f, 0.8f),  // Red color
            buttonTexture,
            Utils::RECT_TOP_LEFT,
            OnQuitClicked
        );
        quitButton.textOffsetX = 0.05f;
        quitButton.textOffsetY = -0.02f;
    }

    void Draw() {
        // Draw background (optional - you can add a background texture or gradient)
        AEGfxSetBackgroundColor(0.1f, 0.1f, 0.15f);  // Dark blue-gray background

        // Draw title
        GraphicsManager::DrawTextUI(
            title,
            -0.3f,   // X position (normalized)
            0.8f,    // Y position (normalized)
            Color(1.0f, 1.0f, 1.0f, 1.0f),  // White color
            2.0f     // Larger text scale for title
        );

        // Draw subtitle or instructions
        std::string instructions = "Click a button or use keyboard shortcuts:\n1 - Play Game | 2 - Map Editor | Q - Quit";
        GraphicsManager::DrawTextUI(
            instructions,
            -0.8f,   // X position
            -0.8f,   // Y position
            Color(0.7f, 0.7f, 0.7f, 1.0f),  // Light gray
            0.7f     // Smaller text scale
        );

        // Draw buttons using GraphicsManager
        Color buttonTextColor(1.0f, 1.0f, 1.0f, 1.0f);  // White text
        GraphicsManager::DrawButton(playButton, buttonTextColor, 1.0f);
        GraphicsManager::DrawButton(mapEditorButton, buttonTextColor, 1.0f);
        GraphicsManager::DrawButton(quitButton, buttonTextColor, 1.0f);

        // Draw version or credits (optional)
        std::string version = "Version 1.0";
        GraphicsManager::DrawTextUI(
            version,
            0.8f,    // X position (bottom right)
            -0.95f,  // Y position
            Color(0.5f, 0.5f, 0.5f, 1.0f),  // Gray
            0.6f     // Small text
        );
    }

    void Update() {
        // Handle button interactions
        bool playHovered = CollisionManager::IsPointInRect(
            playButton.pos.x, playButton.pos.y,
            playButton.width, playButton.height
        );
        bool editorHovered = CollisionManager::IsPointInRect(
            mapEditorButton.pos.x, mapEditorButton.pos.y,
            mapEditorButton.width, mapEditorButton.height
        );
        bool quitHovered = CollisionManager::IsPointInRect(
            quitButton.pos.x, quitButton.pos.y,
            quitButton.width, quitButton.height
        );

        // Check for button clicks
        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            if (playHovered) {
                OnPlayClicked();
            }
            else if (editorHovered) {
                OnMapEditorClicked();
            }
            else if (quitHovered) {
                OnQuitClicked();
            }
        }

        // Keyboard shortcuts
        if (AEInputCheckTriggered(AEVK_1)) {
            OnPlayClicked();
        }
        if (AEInputCheckTriggered(AEVK_2)) {
            OnMapEditorClicked();
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
        GameStateManager::next = GameStateManager::GS_Level_1;
    }

    void OnMapEditorClicked() {
        // Transition to Map Editor
        GameStateManager::next = GameStateManager::GS_Map_Editor;
    }

    void OnQuitClicked() {
        // Quit the game
        GameStateManager::next = GameStateManager::GS_QUIT;
    }
}






