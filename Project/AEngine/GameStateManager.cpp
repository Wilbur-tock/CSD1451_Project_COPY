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

namespace GameStateManager {
    Utils::FuncPtr gsLoad, gsIntialize, gsUpdate, gsDraw, gsFree, gsUnload;
    int current = 0, previous = 0, next = 0;
    // Temporary
    AEGfxTexture* texture;

    void Load() {
        GraphicsManager::Load();

        // Start with main menu
        current = GS_Level_1;
        previous = next = GS_Main_Menu;

        texture = AssetsManager::GetTexture("Assets/square.png");

        // Load all game states
        MainMenu::Load();
        Level1::Load();

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

        // Initialize the main menu since we're starting with it
        //MainMenu::Initialize();
        //Level1::Initialize();
    }

    void Update() {
        // Handle quit state
        if (current == GS_QUIT) {
            // This will cause the game loop to exit
            return;
        }

        // Update Game state function pointers based on current state


        // State transition logic
        if (next != current) {
            // Call Free on the current state
            if (gsFree != nullptr) {
                gsFree();
            }

            // Update state
            previous = current;
            current = next;

            switch (current) {
            case GS_Main_Menu:
                gsLoad = MainMenu::Load;
                gsIntialize = MainMenu::Initialize;
                gsUpdate = MainMenu::Update;
                gsDraw = MainMenu::Draw;
                gsFree = MainMenu::Free;
                gsUnload = MainMenu::Unload;
                break;
            case GS_Map_Editor:
                gsLoad = nullptr;
                gsIntialize = nullptr;
                gsUpdate = MapEditor::MapEditor;
                gsDraw = nullptr;
                gsFree = MapEditor::Exit;
                gsUnload = nullptr;
                break;
            case GS_Level_1:
                gsLoad = Level1::Load;
                gsIntialize = Level1::Initialize;
                gsUpdate = Level1::Update;
                gsDraw = nullptr;
                gsFree = nullptr;
                gsUnload = nullptr;
                break;
            case GS_Level_2:
                // TODO: Implement Level 2
                break;
            case GS_Restart:
                // Restart current level
                next = previous;
                break;
            case GS_Exit:
                // Return to main menu
                next = GS_Main_Menu;
                break;
            }

            // Call Initialize on the new state
            if (gsIntialize != nullptr) {
                gsIntialize();
            }
        }

        // Update current state
        if (gsUpdate != nullptr) {
            gsUpdate();
        }

        // Global controls (available in all states except main menu)
        if (current != GS_Main_Menu) {
            // Toggle debugger
            if (AEInputCheckTriggered(AEVK_P)) {
                Utils::isDebuggerOn = !Utils::isDebuggerOn;
            }

            // Return to main menu with ESC (except when already in main menu)
            if (AEInputCheckTriggered(AEVK_M)) {
                next = GS_Main_Menu;
            }

            // Debugger display
            if (Utils::isDebuggerOn) {
                f32 camX, camY;
                AEGfxGetCamPosition(&camX, &camY);

                std::string str = "\nDebugger ON\nFPS: " + Utils::FormatFloat(static_cast<double>(AEFrameRateControllerGetFrameRate()), 2)
                    + "\nDeltaTime: " + Utils::FormatFloat(static_cast<double>(AEFrameRateControllerGetFrameTime()), 2)
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