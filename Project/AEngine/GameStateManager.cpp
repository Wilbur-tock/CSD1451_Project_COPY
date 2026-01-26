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
    Utils::FuncPtr fpLoad, fpInitalize, fpUpdate, fpDraw, fpFree, fpUnload;
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
            fpLoad = nullptr;
            fpInitalize = nullptr;
            fpUpdate = nullptr;
            fpDraw = nullptr;
            fpFree = nullptr;
            fpUnload = nullptr;
            break;
        case GS_Map_Editor:
            fpLoad = nullptr;
            fpInitalize = nullptr;
            fpUpdate = MapEditor::MapEditor;
            fpDraw = nullptr;
            fpFree = nullptr;
            fpUnload = nullptr;
            break;
        case GS_Level_1:
            fpLoad = Level1::Load;
            fpInitalize = Level1::Initialize;
            fpUpdate = Level1::Update;
            fpDraw = nullptr;
            fpFree = nullptr;
            fpUnload = nullptr;
            break;
        case GS_Level_2:
            break;
        }

        // Update to new state
        if (next != current) {
            previous = current;
            current = next;
            if (fpInitalize != nullptr) {
                fpInitalize();
            }
        }

        if (fpUpdate != nullptr) {
            fpUpdate();
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
            std::string str = "FPS: " + Utils::FormatFloat(static_cast<double>(AEFrameRateControllerGetFrameRate()), 2)
                + "\nDeltaTime: " + Utils::FormatFloat(static_cast<double>(AEFrameRateControllerGetFrameTime()), 2);

            GraphicsManager::DrawTextUI(str, -1.f, -0.9f, Utils::debuggerInfo.first, Utils::debuggerInfo.second);
        }

        // Origin
        //GraphicsManager::DrawGameObject(texture, GraphicsManager::CreateTransformMatrix(0, 0,0,16,16), Color{ 0,0,0 });

        // Update camera (handles movement, clamping, flooring, and setting position)
        camera.Update(static_cast<float>(AEFrameRateControllerGetFrameTime()));
    }

    void Exit() {
    }
}