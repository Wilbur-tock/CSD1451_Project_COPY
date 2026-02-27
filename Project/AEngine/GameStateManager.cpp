#include "GameStateManager.hpp"
#include "MapManager.hpp"
#include "MapEditor.hpp"
#include "CannonManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Level1.hpp"
#include "MainMenu.hpp"
#include "Credits.hpp"
#include "PlayerManager.hpp"
#include "PhysicsManager.hpp"
#include "UIManager.hpp"

namespace GameStateManager {
    Utils::FuncPtr gsLoad, gsIntialize, gsUpdate, gsDraw, gsFree, gsUnload;
    GameState current, previous, next;
    bool setNewState = false;
    float loadingTimer = 0.0f;

    // Loading screen animation variables
    float dotAnimTimer = 0.0f;
    int dotCount = 0;

    TextObject debugText;

    void RenderLoadingScreen() {
        // Reset camera to center for loading screen
        AEGfxSetCamPosition(0.0f, 0.0f);

        // Dark background
        AEGfxSetBackgroundColor(0.1f, 0.1f, 0.15f);

        // Calculate progress (0.0 to 1.0)
        float maxTime = 0.9f;
        float progress = 1.0f - (loadingTimer / maxTime);
        if (progress > 1.0f) progress = 1.0f;

        // Animated dots
        dotAnimTimer += PhysicsManager::g_dt;
        if (dotAnimTimer >= 0.3f) {
            dotAnimTimer = 0.0f;
            dotCount = (dotCount + 1) % 4;
        }

        std::string dots = "";
        for (int i = 0; i < dotCount; i++) {
            dots += ".";
        }

        // Draw "LOADING..." text
        TextObject loadingTextObj{
            "LOADING" + dots,
            1.5f,
            Color(1.0f, 1.0f, 1.0f, 1.0f),
            { -0.15f, 0.2f }
        };
        loadingTextObj.Draw();

        // Loading bar dimensions (in screen pixels)
        float barWidth = 600.0f;
        float barHeight = 40.0f;

        // Center the bar on screen (x=0 is screen center in world coords when cam is at 0,0)
        float barX = 0.0f;  // Center X
        float barY = 15.0f;  // Below the loading text

        // Background bar (full width, dark gray)
        AEMtx33 bgMat = GraphicsManager::CreateTransformMatrix(
            barX - (barWidth / 2.0f),  // Start from left edge
            barY,
            0,
            barWidth,
            barHeight
        );
        GraphicsManager::DrawScreenOverlay(AssetsManager::squareTexture, bgMat, Color(0.2f, 0.2f, 0.2f, 1.0f));

        // Progress bar (fills from left to right)
        float progressWidth = barWidth * progress;
        if (progressWidth > 0) {
            AEMtx33 progressMat = GraphicsManager::CreateTransformMatrix(
                barX - (barWidth / 2.0f),  // Start from same left edge
                barY,
                0,
                progressWidth,  // Only the progress portion
                barHeight
            );
            GraphicsManager::DrawScreenOverlay(AssetsManager::squareTexture, progressMat, Color(0.3f, 0.7f, 1.0f, 1.0f));
        }

        // Percentage text (centered below bar)
        int percentage = static_cast<int>(progress * 100);
        TextObject percentText{
            std::to_string(percentage) + "%",
            1.2f,
            Color(1.0f, 1.0f, 1.0f, 1.0f),
            { -0.03f, -0.15f }
        };
        percentText.Draw();
    }

    void Load() {
        // Start with main menu
        current = previous = GameState::INITIALIZE;
        next = GameState::MAIN_MENU;

        // Load all resources
        MapManager::Load();
        AssetsManager::Load();
        GraphicsManager::Load();
        MainMenu::Load();
        Level1::Load();
        Credits::Load();

        // Load all abilities at the start of the game
        PlayerManager::playerAbilityController.LoadAllAbilities();

        // Initialize camera controller
        CameraManager::camera.Init();
        CameraManager::camera.SetMoveSpeed(1500.0f);

        // Set world boundaries
        float halfW = Config::SCREEN_WIDTH / 2.0f;
        float halfH = Config::SCREEN_HEIGHT / 2.0f;
        CameraManager::camera.SetBounds(
            Config::WORLD_MIN_X + halfW,
            Config::WORLD_MAX_X - halfW,
            Config::WORLD_MIN_Y + halfH,
            Config::WORLD_MAX_Y - halfH
        );

        debugText = { "", Utils::debuggerInfo.first * 0.75f, Utils::debuggerInfo.second, {-1.f, -0.8f} };
    }

    void Update() {
        PhysicsManager::g_dt = static_cast<float>(AEFrameRateControllerGetFrameTime());

        // Handle quit state
        if (current == GameState::QUIT) {
            // This will cause the game loop to exit
            return;
        }

        // State transition logic
        if (next != current) {
            if (!setNewState) {
                loadingTimer = 0.9f; // Initialize timer
                dotAnimTimer = 0.0f; // Reset dot animation
                dotCount = 0;
                setNewState = true;
            }
            else {
                // Is currently Loading
                loadingTimer -= PhysicsManager::g_dt;

                // Render loading screen during transition
                RenderLoadingScreen();

                if (loadingTimer <= 0.0f) {
                    // Call Free on the current state
                    if (gsFree != nullptr) {
                        gsFree();
                    }

                    // Update state
                    previous = current;
                    current = next;
                    setNewState = false;

                    // Update Game state function pointers based on current state
                    switch (current) {
                    case GameState::MAIN_MENU:
                        gsLoad = MainMenu::Load;
                        gsIntialize = MainMenu::Initialize;
                        gsUpdate = MainMenu::Update;
                        gsDraw = MainMenu::Draw;
                        gsFree = MainMenu::Free;
                        gsUnload = MainMenu::Unload;
                        break;
                    case GameState::MAP_EDITOR:
                        gsLoad = nullptr;
                        gsIntialize = MapEditor::Initialize;
                        gsUpdate = MapEditor::MapEditor;
                        gsDraw = nullptr;
                        gsFree = MapEditor::Exit;
                        gsUnload = nullptr;
                        break;
                    case GameState::LEVEL_1:
                        gsLoad = Level1::Load;
                        gsIntialize = Level1::Initalize;
                        gsUpdate = Level1::Update;
                        gsDraw = nullptr;
                        gsFree = nullptr;
                        gsUnload = nullptr;
                        break;
                    case GameState::LEVEL_2:
                        // TODO: Implement Level 2
                        break;
                    case GameState::CREDITS:
                        gsLoad = Credits::Load;
                        gsIntialize = Credits::Initialize;
                        gsUpdate = Credits::Update;
                        gsDraw = Credits::Draw;
                        gsFree = Credits::Free;
                        gsUnload = Credits::Unload;
                        break;
                    case GameState::RESTART:
                        // Restart current level
                        next = previous;
                        break;
                    case GameState::EXIT:
                        // Return to main menu
                        next = GameState::MAIN_MENU;
                        break;
                    }

                    // Call Initialize on the new state
                    if (gsIntialize != nullptr) {
                        gsIntialize();
                    }
                }
                // Return early during loading - don't update the game state
                return;
            }
        }

        // Update current state
        if (gsUpdate != nullptr) {
            gsUpdate();
        }

        // Toggle debugger
        if (AEInputCheckTriggered(AEVK_P)) {
            Utils::isDebuggerOn = !Utils::isDebuggerOn;
        }

        // Return to main menu with ESC (except when already in main menu)
        if (AEInputCheckTriggered(AEVK_M)) {
            next = GameState::MAIN_MENU;
        }

        // Reload assets
        if (AEInputCheckCurr(AEVK_LCTRL)) {
            if (AEInputCheckCurr(AEVK_R)) {
                Load();
            }
        }

        // Debugger display
        if (Utils::isDebuggerOn) {
            UIManager::Panel panel{ {250, 75}, {1.0f,1.0f,1.0f, 0.75f}, Utils::RectMode::RECT_BOTTOM_LEFT };
            panel.Draw();

            f32 camX, camY;
            AEGfxGetCamPosition(&camX, &camY);

            debugText.textContent = "\n[P] Debugger ON\nFPS: " + Utils::FormatFloat(AEFrameRateControllerGetFrameRate(), 2)
                + "\nDeltaTime: " + Utils::FormatFloat(PhysicsManager::g_dt, 2)
                + "\n[M] Main Menu" + "\nCamera State: " + CameraManager::camera.PrintState();

            debugText.Draw();
        }

        // Update camera (handles movement, clamping, flooring, and setting position)
        CameraManager::camera.Update(PhysicsManager::g_dt);
    }

    void Exit() {
        // Call unload for all states
        MainMenu::Unload();
        AssetsManager::Unload();
        GraphicsManager::Unload();
    }
}