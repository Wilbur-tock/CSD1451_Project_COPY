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
    // State variables
    Utils::FuncPtr gsLoad, gsIntialize, gsUpdate, gsDraw, gsFree, gsUnload; // Six function pointers

    GameState current, previous, next; // All three start uninitialised

    bool setNewState = false;   // One-shot flag that becomes true the first frame a transition is detected
                                // Prevents Loading::Init() from resetting the loading timer on every frame of the transition

    TextObject debugText;       // Reusable TextObject for the debugger overlay
    

    // IsGameplayState (private helper), returns true if state supports pausing
    static bool IsGameplayState() {
        return current == GameState::LEVEL_1
            || current == GameState::LEVEL_2
            || current == GameState::MAP_EDITOR;
    }

    void Load() {
        // Start with main menu
        current = previous = GameState::INITIALIZE;
        next = GameState::MAIN_MENU;

        // Clear function pointers
        gsLoad = gsIntialize = gsUpdate = gsDraw = gsFree = gsUnload = nullptr;

        // Load all resources
        AssetsManager::Load(); // Must run before any state that calls GetTexture, including MainMenu::Load() below

        UICursor::Load();               // Load the custom cursor texture
                                        // Done globally so the cursor is always available regardless of state.
        GraphicsManager::Load();        // Initialise the AEGfx rendering pipeline
        LevelManager::Load();           // Pre-load level data structures so level transitions are fast
        MapManager::Load();             // Load the tile map data needed by both LevelManager and MapEditor
        MainMenu::Load();               // Pre-load the main menu's button and background textures into the asset cache
        Credits::Load();                // Pre-load any assets required by the credits screen

        AudioManager::Init();           // Initialise the FMOD audio engine
        AudioManager::SetVolume(0.35f); // Set the master volume to 35% at startup

        // Initialize camera controller
        CameraManager::camera.Init();   // Initialise the camera controller with default values
        CameraManager::camera.SetMoveSpeed(1500.0f); // Set how fast the camera scrolls in world units per second

        // Set world boundariesm, prevents player from scolling outside the playable area
        CameraManager::camera.SetBounds(
            Config::WORLD_MIN_X, // Left boundary
            Config::WORLD_MAX_X, // Right boundary
            Config::WORLD_MIN_Y, // Bottom boundary
            Config::WORLD_MAX_Y  // Top boundary
        );

        setNewState = false; // Explicitly clear the transition flag at startup so the first Update() starts cleanly

        Loading::Init();            // Initialise the loading screen timer and animation variables

        PauseMenu::Init();          // Build the pause menu's buttons and text objects once at startup, prevents construction delay
        Settings::Load();           // Load assets needed by the settings screen
        Settings::Initialize();     // Build the settings screen's UI elements
    }

    void Update() {
        // Read the real elapsed time for this frame from the AE frame-rate controller
        // so loading timers and audio keep running correctly even when the game is frozen
        float rawDt = static_cast<float>(AEFrameRateControllerGetFrameTime());

        PhysicsManager::g_dt = rawDt; // Copy rawDt into the physics delta-time 
                                      // Setting it here first ensures physics runs at full speed during normal gameplay

        AudioManager::Update(); // Pump the FMOD audio engine each frame. Must be called regardless of pause or transition state

        // Handle quit -- skip loading screen entirely 
        // When the Quit button sets next = QUIT, the very next frame hits the new check, 
        // immediately assigns current = QUIT, and returns no loading screen, no transition timer.
        if (next == GameState::QUIT || current == GameState::QUIT) {
            current = next;  // Skips transition
            if (gsDraw != nullptr) gsDraw(); // Renders one last frame
            return;
        }

        // STATE TRANSITION (LOADING SCREEN) 
        if (next != current) {
            // Unpause when switching states so the new state starts clean
            Utils::isPaused = false;
            Utils::isSettingsOpen = false;

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
                    gsLoad = LevelManager::LoadLevel;
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
                    gsIntialize = Settings::Initialize;
                    gsUpdate = Settings::Update;
                    gsDraw = Settings::Draw;
                    gsFree = Settings::Free;
                    break;

                case GameState::RESTART:
                    next = previous;
                    current = next;
                    break;

                case GameState::EXIT:
                    next = GameState::MAIN_MENU;
                    break;

                default:
                    break;
                }

                // IMPORTANT: call gsLoad before gsInitialize
                // Initialize may reference assets that Load provides.
                if (gsLoad != nullptr) {
                    gsLoad();
                }

                // Call the new state's Initialize() to build its scene objects using the loaded assets
                if (gsIntialize != nullptr) {
                    gsIntialize();
                }

                // Start the correct background music for the new state
                // Placed after gsLoad and gsIntialize so audio starts only once the state is fully ready
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
            }

            // Always stop normal update/camera logic while transitioning
            return;
        }

        // PAUSE MENU (ESC)
        // Toggle pause only during gameplay states
        if (IsGameplayState() && !Utils::isInstructionOn && !Utils::isResultScreenOn) {
            if (AEInputCheckTriggered(AEVK_ESCAPE)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                Utils::isPaused = !Utils::isPaused;
            }
        }

        if (Utils::isPaused) {
            PhysicsManager::g_dt = 0.0f;  // freeze game time

            // Render the current state underneath (draw only, no input)
            if (gsDraw != nullptr) {
                gsDraw();
            }

            // Show either the settings overlay or the pause menu
            if (Utils::isSettingsOpen) {
                Settings::Update();   // Setting draws its own cursor before restoring camera
            }
            else {
                PauseMenu::Update();
                UICursor::Draw();     // Draw the custom cursor on top of everything after PauseMenu renders (prev bug)
            }

            return;
        }

        // UNPAUSE COOLDOWN
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

        // Toggle instructions popup (only when NOT loading)
        if (AEInputCheckTriggered(AEVK_I)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
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
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
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
                + "\n[M] Main Menu";
            debugText.Draw();
        }

        UICursor::Draw();

        // Instructions overlay (draw last so it appears on top)
        if (Utils::isInstructionOn) {
            UIInstructionsOverlay::Draw(current);
            return; // block camera movement while popup is open
        }

        // Update the camera using g_dt 
        CameraManager::camera.Update(PhysicsManager::g_dt); // freezes when physics freezes, consistent
    }

    // Releases all globally owned resources in reverse dependency order
    void Exit() {
        MainMenu::Unload();
        AssetsManager::Unload();
        UICursor::Unload();
        GraphicsManager::Unload();
        MapManager::Unload();
        AudioManager::Shutdown();
    }
}