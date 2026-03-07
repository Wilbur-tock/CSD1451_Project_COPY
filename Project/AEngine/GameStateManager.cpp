#include "GameStateManager.hpp"
#include "MapManager.hpp"
#include "MapEditor.hpp"
#include "CannonManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "LevelManager.hpp"
#include "MainMenu.hpp"
#include "Credits.hpp"
#include "PhysicsManager.hpp"
#include "UIManager.hpp"
#include "UICursor.hpp"
#include "AudioManager.hpp"
#include "InstructionsOverlay.hpp"

namespace GameStateManager {
    Utils::FuncPtr gsLoad, gsIntialize, gsUpdate, gsDraw, gsFree, gsUnload;
    GameState current, previous, next;
    bool setNewState = false;
    float loadingTimer = 0.0f;

    // Loading screen animation variables
    float dotAnimTimer = 0.0f;
    int dotCount = 0;

    void RenderLoadingScreen() {
        // Reset camera to center for loading screen
        AEGfxSetCamPosition(0.0f, 0.0f);

        // Dark background
        AEGfxSetBackgroundColor(0.1f, 0.1f, 0.15f);

        // Calculate progress (0.0 to 1.0)
        float maxTime = 0.9f;
        float progress = 1.0f - (loadingTimer / maxTime);
        if (progress > 1.0f) progress = 1.0f;
        if (progress < 0.0f) progress = 0.0f;

        // Animated dots (do NOT depend on paused g_dt in case you pause elsewhere)
        dotAnimTimer += static_cast<float>(AEFrameRateControllerGetFrameTime());
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

        // Loading bar dimensions
        float barWidth = 600.0f;
        float barHeight = 40.0f;

        float barX = 0.0f;
        float barY = 15.0f;

        // Background bar
        AEMtx33 bgMat = GraphicsManager::CreateTransformMatrix(
            barX - (barWidth / 2.0f),
            barY,
            0,
            barWidth,
            barHeight
        );
        GraphicsManager::DrawScreenOverlay(AssetsManager::squareTexture, bgMat, Color(0.2f, 0.2f, 0.2f, 1.0f));

        // Progress bar
        float progressWidth = barWidth * progress;
        if (progressWidth > 0.0f) {
            AEMtx33 progressMat = GraphicsManager::CreateTransformMatrix(
                barX - (barWidth / 2.0f),
                barY,
                0,
                progressWidth,
                barHeight
            );
            GraphicsManager::DrawScreenOverlay(AssetsManager::squareTexture, progressMat, Color(0.3f, 0.7f, 1.0f, 1.0f));
        }

        // Percentage text
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

        // Clear function pointers
        gsLoad = gsIntialize = gsUpdate = gsDraw = gsFree = gsUnload = nullptr;

        // Load all resources
        AssetsManager::Load();
        UICursor::Load();
        GraphicsManager::Load();
        LevelManager::Load();
        MapManager::Load();
        MainMenu::Load();
        Credits::Load();
		AudioManager::Init();

        // Initialize camera controller
        CameraManager::camera.Init();
        CameraManager::camera.SetMoveSpeed(1500.0f);

        // Set world boundaries
        CameraManager::camera.SetBounds(
            Config::WORLD_MIN_X,
            Config::WORLD_MAX_X,
            Config::WORLD_MIN_Y,
            Config::WORLD_MAX_Y
        );

        setNewState = false;
        loadingTimer = 0.0f;
        dotAnimTimer = 0.0f;
        dotCount = 0;
    }

    void Update() {
        float rawDt = static_cast<float>(AEFrameRateControllerGetFrameTime());
        PhysicsManager::g_dt = rawDt;

        // Handle quit state
        if (current == GameState::QUIT) {
            return;
        }

        // ================== STATE TRANSITION (LOADING SCREEN) ==================
        if (next != current) {
            // Do not allow instructions popup to interfere with loading
            Utils::isInstructionOn = false;

            // Start transition once
            if (!setNewState) {
                loadingTimer = 0.9f;
                dotAnimTimer = 0.0f;
                dotCount = 0;
                setNewState = true;
            }

            // IMPORTANT: decrement with rawDt (NOT PhysicsManager::g_dt)
            loadingTimer -= rawDt;

            // Render loading screen during transition
            RenderLoadingScreen();

            if (loadingTimer <= 0.0f) {

                // Free old state
                if (gsFree != nullptr) {
                    gsFree();
                }

                previous = current;
                current = next;
                setNewState = false;

                // Update Game state function pointers based on current state
                switch (current) {
                case GameState::MAIN_MENU:
                    gsIntialize = MainMenu::Initialize;
                    gsUpdate = MainMenu::Update;
                    gsDraw = MainMenu::Draw;
                    gsFree = MainMenu::Free;
                    break;

                case GameState::MAP_EDITOR:
                    gsIntialize = MapEditor::Initialize;
                    gsUpdate = MapEditor::Update;
                    gsDraw = nullptr;
                    gsFree = MapEditor::Exit;
                    break;

                case GameState::LEVEL_1:
                case GameState::LEVEL_2:
                    LevelManager::SelectLevel(static_cast<int>(current) - static_cast<int>(GameState::LEVEL_1));
                    gsIntialize = LevelManager::InitializeLevel;
                    gsUpdate = LevelManager::UpdateLevel;
                    gsDraw = nullptr;
                    gsFree = LevelManager::FreeLevel;
                    break;

                case GameState::CREDITS:
                    gsIntialize = Credits::Initialize;
                    gsUpdate = Credits::Update;
                    gsDraw = Credits::Draw;
                    gsFree = Credits::Free;
                    break;

                case GameState::RESTART:
                    next = previous;
                    break;

                case GameState::EXIT:
                    next = GameState::MAIN_MENU;
                    break;

                default:
                    break;
                }

                if (gsIntialize != nullptr) {
                    gsIntialize();
                }
                switch (current) {
                case GameState::MAIN_MENU:
                    AudioManager::PlayBGM(AudioManager::Track::MainMenu);
                    break;
                case GameState::LEVEL_1:
                case GameState::LEVEL_2:
                    AudioManager::PlayBGM(AudioManager::Track::Level);
                    break;
                case GameState::CREDITS:
                    AudioManager::PlayBGM(AudioManager::Track::Credits);
                    break;
                default:
                    // Optional: stop music for other states
                    // AudioManager::StopBGM();
                    break;
                }

                // Apply camera immediately after state switch
                CameraManager::camera.Update(0.0f);
            }

            // Always stop normal update/camera logic while transitioning
            return;
        }
        // ======================================================================

        // Toggle instructions popup (only when NOT loading)
        if (AEInputCheckTriggered(AEVK_I)) {
            Utils::isInstructionOn = !Utils::isInstructionOn;
        }

        // Pause time while instructions are open
        if (Utils::isInstructionOn) {
            PhysicsManager::g_dt = 0.0f;
        }

        // Update current state
        if (gsUpdate != nullptr) {
            gsUpdate();
        }

        // Global shortcuts should not trigger while instructions are open
        if (!Utils::isInstructionOn) {
            // Toggle debugger
            if (AEInputCheckTriggered(AEVK_P)) {
                Utils::isDebuggerOn = !Utils::isDebuggerOn;
            }

            // Return to main menu
            if (AEInputCheckTriggered(AEVK_M)) {
                next = GameState::MAIN_MENU;
            }
        }

        // Debugger display
        if (Utils::isDebuggerOn && !Utils::isInstructionOn) {
            UIManager::Panel debuggerPanel{ {200, 75}, {1.0f,1.0f,1.0f, 0.75f}, Utils::RectMode::RECT_BOTTOM_LEFT };
            TextObject debugText{ "", Utils::debuggerInfo.first * 0.75f, Utils::debuggerInfo.second, {-1.f, -0.81f} };

            debuggerPanel.Draw();

            debugText.textContent = "\n[P] Debugger ON\nFPS: " + Utils::FormatFloat(AEFrameRateControllerGetFrameRate(), 2)
                + "\nDeltaTime: " + Utils::FormatFloat(rawDt, 2);
                //+ "\n[M] Main Menu" + "\nCamera State: " + CameraManager::camera.PrintState()
                //+ "\n Zoom: " + Utils::FormatFloat(CameraManager::camera.GetZoom(), 2)
                //+ "\n" + CameraManager::camera.PrintVisibleTiles();

            debugText.Draw();
        }

        UICursor::Draw();

        // Instructions overlay (draw last so it appears on top)
        if (Utils::isInstructionOn) {
            UIInstructionsOverlay::Draw(current);
            return; // block camera movement while popup is open
        }

        // Update camera
        CameraManager::camera.Update(PhysicsManager::g_dt);
    }

    void Exit() {
        MainMenu::Unload();
        AssetsManager::Unload();
        UICursor::Unload();
        GraphicsManager::Unload();
        MapManager::Unload();
		AudioManager::Shutdown();
    }
}