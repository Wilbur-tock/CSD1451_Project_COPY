#include "pch.hpp"
#include "MapManager.hpp"
#include "MapEditor.hpp"
#include "CannonManager.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "Level1.hpp"
#include "CameraManager.hpp"
#include "MainMenu.hpp"
#include "Credits.hpp"

namespace GameStateManager {
    Utils::FuncPtr gsLoad, gsIntialize, gsUpdate, gsDraw, gsFree, gsUnload;
    GameState current, previous, next;
    bool setNewState = false;
    float loadingTimer = 0.0f;
    float dt = 0.0f;
    // Temporary
    AEGfxTexture* texture;

    void Load() {
        // Start with main menu
        current = previous = GS_INIT;
        next = GS_MAIN_MENU;

        texture = AssetsManager::GetTexture("Assets/square.png");

        // Load all game states
        GraphicsManager::Load();
        MainMenu::Load();
        Level1::Load();
        Credits::Load();

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
    }

    void Update() {
        dt = static_cast<float>(AEFrameRateControllerGetFrameTime());

        // Handle quit state
        if (current == GS_QUIT) {
            // This will cause the game loop to exit
            return;
        }

        // State transition logic
        if (next != current) {
            if (!setNewState) {
                loadingTimer = 0.75f;
                setNewState = true;
            }
            else {
                loadingTimer -= dt;

                if (loadingTimer <= 0.0f){
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
                    case GS_MAIN_MENU:
                        gsLoad = MainMenu::Load;
                        gsIntialize = MainMenu::Initialize;
                        gsUpdate = MainMenu::Update;
                        gsDraw = MainMenu::Draw;
                        gsFree = MainMenu::Free;
                        gsUnload = MainMenu::Unload;
                        break;
                    case GS_MAP_EDITOR:
                        gsLoad = nullptr;
                        gsIntialize = nullptr;
                        gsUpdate = MapEditor::MapEditor;
                        gsDraw = nullptr;
                        gsFree = MapEditor::Exit;
                        gsUnload = nullptr;
                        break;
                    case GS_LEVEL_1:
                        gsLoad = Level1::Load;
                        gsIntialize = Level1::Initialize;
                        gsUpdate = Level1::Update;
                        gsDraw = nullptr;
                        gsFree = nullptr;
                        gsUnload = nullptr;
                        break;
                    case GS_LEVEL_2:
                        // TODO: Implement Level 2
                        break;
                    case GS_CREDITS:
                        gsLoad = Credits::Load;
                        gsIntialize = Credits::Initialize;
                        gsUpdate = Credits::Update;
                        gsDraw = Credits::Draw;
                        gsFree = Credits::Free;
                        gsUnload = Credits::Unload;
                        break;
                    case GS_RESTART:
                        // Restart current level
                        next = previous;
                        break;
                    case GS_EXIT:
                        // Return to main menu
                        next = GS_MAIN_MENU;
                        break;
                    }

                    // Call Initialize on the new state
                    if (gsIntialize != nullptr) {
                        gsIntialize();
                    }
                }
            }
        }

        // Update current state
        if (gsUpdate != nullptr) {
            gsUpdate();
        }

        // Global controls (available in all states except main menu)
        if (current != GS_MAIN_MENU) {
            // Toggle debugger
            if (AEInputCheckTriggered(AEVK_P)) {
                Utils::isDebuggerOn = !Utils::isDebuggerOn;
            }

            // Return to main menu with ESC (except when already in main menu)
            if (AEInputCheckTriggered(AEVK_M)) {
                next = GS_MAIN_MENU;
            }

            // Debugger display
            if (Utils::isDebuggerOn) {
                f32 camX, camY;
                AEGfxGetCamPosition(&camX, &camY);

                std::string str = "\n[P] Debugger ON\nFPS: " + Utils::FormatFloat(AEFrameRateControllerGetFrameRate(), 2)
                    + "\nDeltaTime: " + Utils::FormatFloat(dt, 2)
                    + "\nCamera Pos:\nX: " + Utils::FormatFloat(camX, 2) + " Y: " + Utils::FormatFloat(camY, 2)
                    + "\n\nControls:\nP - Toggle Debugger\nM - Main Menu";

                GraphicsManager::DrawTextUI(str, -1.f, -0.75f, Utils::debuggerInfo.first, Utils::debuggerInfo.second);
            }

            // Update camera (handles movement, clamping, flooring, and setting position)
            CameraManager::camera.Update(static_cast<float>(AEFrameRateControllerGetFrameTime()));
        }
        else {
            // In main menu, reset camera to center
            AEGfxSetCamPosition(0.0f, 0.0f);
        }
    }

    void Exit() {
        // Call unload for all states
        MainMenu::Unload();

        // Free camera resources if needed
    }
}