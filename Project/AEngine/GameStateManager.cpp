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

    // Camera instance
    CameraController camera;

    void Load() {
        GraphicsManager::Load();
        current = 0;
        previous = next = GS_Level_1;
        texture = AssetsManager::GetTexture("Assets/square.png");
        Level1::Load();

        // Initialize camera controller
        camera.Init();
        camera.SetMoveSpeed(1500.0f);

        // Set world boundaries
        float halfW = Config::SCREEN_WIDTH / 2.0f;
        float halfH = Config::SCREEN_HEIGHT / 2.0f;
        camera.SetBounds(
            Config::WORLD_MIN_X + halfW,
            Config::WORLD_MAX_X - halfW,
            Config::WORLD_MIN_Y + halfH,
            Config::WORLD_MAX_Y - halfH
        );
    }

    bool mapEditorOn = true;

    void Update() {
        // Update Game state
        switch (current) {
        case GS_Main_Menu:
            gsLoad = nullptr;
            gsIntialize = nullptr;
            gsUpdate = nullptr;
            gsDraw = nullptr;
            gsFree = nullptr;
            gsUnload = nullptr;
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
            break;
        }

        // Update to new state
        if (next != current) {
            if (gsFree != nullptr) {
                gsFree();
            }

            // Set next state settings
            previous = current;
            current = next;
            if (gsIntialize != nullptr) {
                gsIntialize();
            }
        }

        if (gsUpdate != nullptr) {
            gsUpdate();
        }

        if (AEInputCheckTriggered(AEVK_Q)) {
            mapEditorOn = !mapEditorOn;
        }

        if (mapEditorOn) {
            next = GS_Map_Editor;
        }
        else {
            next = GS_Level_1;
        }

        if (AEInputCheckTriggered(AEVK_P)) {
            Utils::isDebuggerOn = !Utils::isDebuggerOn;
        }

        // Debugger
        if (Utils::isDebuggerOn) {
            f32 camX, camY;
            AEGfxGetCamPosition(&camX, &camY);

            std::string str = "\nDebugger ON\nFPS: " + Utils::FormatFloat(static_cast<double>(AEFrameRateControllerGetFrameRate()), 2)
                + "\nDeltaTime: " + Utils::FormatFloat(static_cast<double>(AEFrameRateControllerGetFrameTime()), 2)
                + +"\nCamera Pos:\nX: " + Utils::FormatFloat(camX, 2) + " Y: " + Utils::FormatFloat(camY, 2);

            GraphicsManager::DrawTextUI(str, -1.f, -0.75f, Utils::debuggerInfo.first, Utils::debuggerInfo.second);
        }

        // Origin
        //GraphicsManager::DrawGameObject(texture, GraphicsManager::CreateTransformMatrix(0, 0,0,16,16), Color{ 0,0,0 });

        // Update camera (handles movement, clamping, flooring, and setting position)
        camera.Update(static_cast<float>(AEFrameRateControllerGetFrameTime()));
    }

    void Exit() {
    }
}