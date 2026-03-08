#include "GameStateManager.hpp"
#include "MapManager.hpp"
#include "MapEditor.hpp"
#include "CannonManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "LevelManager.hpp"
#include "MainMenu.hpp"
#include "Credits.hpp"
#include "Settings.hpp"
#include "PhysicsManager.hpp"
#include "UIManager.hpp"
#include "UICursor.hpp"
#include "AudioManager.hpp"
#include "InstructionsOverlay.hpp"
#include "Loading.hpp"
#include "PauseMenu.hpp"                                           

namespace GameStateManager {
    Utils::FuncPtr gsLoad, gsIntialize, gsUpdate, gsDraw, gsFree, gsUnload;
    GameState current, previous, next;
    bool setNewState = false;

    TextObject debugText;

    // Helper: returns true when the current state supports pausing       // <-- NEW
    static bool IsGameplayState() {                                       // <-- NEW
        return current == GameState::LEVEL_1                              // <-- NEW
            || current == GameState::LEVEL_2                              // <-- NEW
            || current == GameState::MAP_EDITOR;                          // <-- NEW
    }                                                                     // <-- NEW

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
        AudioManager::SetVolume(0.35f);

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
        Loading::Init();

        PauseMenu::Init();                                                // <-- NEW
    }

    void Update() {
        float rawDt = static_cast<float>(AEFrameRateControllerGetFrameTime());
        PhysicsManager::g_dt = rawDt;

        // Handle quit -- skip loading screen entirely 
        // When the Quit button sets next = QUIT, the very next frame hits the new check, 
        // immediately assigns current = QUIT, and returns no loading screen, no transition timer.
        if (next == GameState::QUIT || current == GameState::QUIT) {
            current = next;
            if (gsDraw != nullptr) gsDraw(); // render one last frame
            return;
        }

        // ================== STATE TRANSITION (LOADING SCREEN) ==================
        if (next != current) {
            // Unpause when switching states so the new state starts clean
            Utils::isPaused = false;

            // Do not allow instructions popup to interfere with loading
            Utils::isInstructionOn = false;

            // Start transition once
            if (!setNewState) {
                Loading::Init();         // resets timer + dot variables
                setNewState = true;
            }

            // IMPORTANT: decrement with rawDt (NOT PhysicsManager::g_dt)
            Loading::loadingTimer -= rawDt;

            // Render loading screen during transition
            Loading::Render();

            if (Loading::loadingTimer <= 0.0f) {

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

                case GameState::SETTINGS:
                    gsLoad = Settings::Load;
                    gsIntialize = Settings::Initialize;
                    gsUpdate = Settings::Update;
                    gsDraw = Settings::Draw;
                    gsFree = Settings::Free;
                    gsUnload = Settings::Unload;
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

                // IMPORTANT: call gsLoad before gsInitialize
                if (gsLoad != nullptr) {
                    gsLoad();
                }

                if (gsIntialize != nullptr) {
                    gsIntialize();
                }
                switch (current) {
                case GameState::MAIN_MENU:
                    AudioManager::PlayBGM(AudioManager::Track::MainMenu);
                    break;

                case GameState::MAP_EDITOR:
                    AudioManager::PlayBGM(AudioManager::Track::MapEditor);
                    break;

                case GameState::LEVEL_1:
                case GameState::LEVEL_2:
                    AudioManager::PlayBGM(AudioManager::Track::Level);
                    break;

                case GameState::CREDITS:
                    AudioManager::PlayBGM(AudioManager::Track::Credits);
                    break;

                case GameState::QUIT:
                    AudioManager::StopBGM();
                    break;

                default:
                    AudioManager::StopBGM();
                    break;
                }

                // Apply camera immediately after state switch
                CameraManager::camera.Update(0.0f);

                // ------- Return-to-pause: re-open pause menu after loading ----
                // Only consume the flag when we land back in a gameplay state.
                // If we just entered SETTINGS, keep the flag alive so it
                // triggers when Settings navigates back to the level.
                if (Utils::returnToPause && IsGameplayState()) {
                    Utils::returnToPause = false;
                    Utils::isPaused = true;
                }
                // --------------------------------------------------------------
            }

            // Always stop normal update/camera logic while transitioning
            return;
        }
        // ======================================================================

        // =================== PAUSE MENU (ESC) ================================
        // Toggle pause only during gameplay states
        if (IsGameplayState() && !Utils::isInstructionOn) {
            if (AEInputCheckTriggered(AEVK_ESCAPE)) {
                Utils::isPaused = !Utils::isPaused;
            }
        }

        if (Utils::isPaused) {
            PhysicsManager::g_dt = 0.0f;  // freeze game time

            // Render the current state underneath (draw only, no input)
            if (gsDraw != nullptr) {
                gsDraw();
            }

            // Draw and handle the pause menu on top
            PauseMenu::Update();
            UICursor::Draw();
            return;
        }
        // ======================================================================

        // =================== UNPAUSE COOLDOWN =================================
        // Brief input-eating delay after resuming so the Resume click
        // does not register as a game action (e.g. firing a bullet).
        if (Utils::unpauseCooldown > 0.0f) {
            Utils::unpauseCooldown -= rawDt;
            PhysicsManager::g_dt = 0.0f;  // keep game frozen during cooldown

            // Render only (no input) so screen is not blank
            if (gsDraw != nullptr) {
                gsDraw();
            }

            UICursor::Draw();
            return;  // skip all normal input handling this frame
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
            UIManager::Panel debuggerPanel{ {200, 75}, {1.0f,1.0f,1.0f, 0.75f}, Utils::AnchorMode::BOTTOM_LEFT };
            debugText = { "", Utils::debuggerInfo.first * 0.75f, Utils::debuggerInfo.second, {-1.f, -0.81f} };

            debuggerPanel.Draw();

            debugText.textContent = "\n[P] Debugger ON\nFPS: " + Utils::FormatFloat(AEFrameRateControllerGetFrameRate(), 2)
                + "\nDeltaTime: " + Utils::FormatFloat(rawDt, 2)
                +"\n[M] Main Menu";
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