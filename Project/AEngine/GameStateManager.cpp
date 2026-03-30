/*
Authors: Dayrius, Nixon, Bryan
Description: Manage all the game states in the game and transition between each states

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "GameStateManager.hpp"

#include "MapManager.hpp"
#include "MapEditor.hpp"
#include "CannonManager.hpp"
#include "CameraManager.hpp"
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
#include "Guide.hpp"
#include "LevelSelect.hpp"
#include "GameTrackingManager.hpp"
#include "ParticlesManager.hpp"

namespace GameStateManager {
    // State variables
    Utils::FuncPtr gsLoad, gsIntialize, gsUpdate, gsDraw, gsFree, gsUnload;

    GameState current, previous, next;

    bool setNewState = false;

    static bool IsGameplayState() {
        return current >= GameState::MAP_EDITOR && current <= GameState::LEVEL_3;
    }

    static void UpdateState();
    static void DebugCommands();
    static void DebugUI();
    static void SetAndPlayBGM();

    void Load() {
        GameTrackingManager::gameData.GameStart();

        // Used to set randomiser seed
        srand(static_cast<unsigned int>(time(NULL)));

        current = previous = GameState::INITIALIZE;
        next = GameState::MAIN_MENU;

        gsLoad = gsIntialize = gsUpdate = gsDraw = gsFree = gsUnload = nullptr;
        setNewState = false;

        // =============== WARNING DON'T CHANGE THE LOAD ORDER ===============
        AssetsManager::Load();
        GraphicsManager::Load();

        LevelManager::LoadAllLevels();
        MapManager::LoadTexture();
        MainMenu::Load();
        Settings::Load();
        AudioManager::Load();

        UICursor::Load();
        LevelSelect::Load();
        Credits::Load();
        Guide::Load();

        // Initialize objects after loading

        Loading::Init();
        PauseMenu::Init();

        CameraManager::camera.Init();
        ParticleManager::particleController.Init();

        UIInstructionsOverlay::Initialize();
    }

    void Update() {
        float rawDt = static_cast<float>(AEFrameRateControllerGetFrameTime());
        PhysicsManager::g_dt = rawDt;

        AudioManager::Update();

        // Loading and changing of state
        if (next != current) {
            Utils::isPaused = false;
            Utils::isSettingsOpen = false;
            Utils::isInstructionOn = false;

            if (!setNewState) {
                if (next != GameState::QUIT) {
                    Loading::Init();
                }
                setNewState = true;
            }
            else {
                // Display loading screen
                if (Loading::loadingTimer > 0.0f) {
                    Loading::loadingTimer -= rawDt;
                    Loading::Render();
                }
                else {
                    UpdateState();
                }
            }

            return;
        }

        // Inputs
        if (IsGameplayState() && !Utils::isInstructionOn && !Utils::isResultScreenOn) {
            if (AEInputCheckTriggered(AEVK_ESCAPE)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                Utils::isPaused = !Utils::isPaused;
            }
        }

        if (Utils::isPaused) {
            PhysicsManager::g_dt = 0.0f;

            if (gsDraw != nullptr) {
                gsDraw();
            }

            if (Utils::isSettingsOpen) {
                Settings::Update();
            }
            else {
                PauseMenu::Update();
                UICursor::Draw();
            }

            return;
        }

        // UNPAUSE COOLDOWN
        if (Utils::unpauseCooldown > 0.0f) {
            Utils::unpauseCooldown -= rawDt;
            PhysicsManager::g_dt = 0.0f;

            if (gsDraw != nullptr) {
                gsDraw();
            }

            UICursor::Draw();
            return;
        }

        if (Utils::isInstructionOn) {
            PhysicsManager::g_dt = 0.0f;
        }

        // Update game play loop
        if (gsUpdate && !Utils::isInstructionOn) {
            gsUpdate();
        }

        DebugCommands();
        DebugUI();
        UICursor::Draw();

        if (Utils::isInstructionOn) {
            UIInstructionsOverlay::Draw();
            return;
        }

        // Camera movment
        CameraManager::camera.Update(PhysicsManager::g_dt);
    }

    void Exit() {
        GameTrackingManager::gameData.GameEnd();
        AssetsManager::Unload();

        MainMenu::Unload();
        LevelSelect::Unload();
        LevelManager::UnloadLevel();
        Guide::Unload();
        UICursor::Unload();
        MapManager::Unload();
        AudioManager::Shutdown();
        ParticleManager::particleController.Shutdown();

        GraphicsManager::Unload();
    }

    static void UpdateState() {
        // Free previous state
        if (gsFree != nullptr) {
            gsFree();
        }

        // Assigned new state
        previous = current;
        current = next;
        setNewState = false;

        switch (current) {
        case GameState::MAIN_MENU:
            gsIntialize = MainMenu::Initialize;
            gsUpdate = MainMenu::Update;
            gsDraw = MainMenu::Draw;
            gsFree = MainMenu::Free;
            break;

        case GameState::LEVEL_SELECT:
            gsIntialize = LevelSelect::Initialize;
            gsUpdate = LevelSelect::Update;
            gsDraw = LevelSelect::Draw;
            gsFree = LevelSelect::Free;
            break;

        case GameState::MAP_EDITOR:
            gsIntialize = MapEditor::Initialize;
            gsUpdate = MapEditor::Update;
            gsDraw = nullptr;
            gsFree = MapEditor::Exit;
            break;

        case GameState::LEVEL_0:
        case GameState::LEVEL_1:
        case GameState::LEVEL_2:
        case GameState::LEVEL_3:
            LevelManager::SelectLevel(static_cast<int>(current) - static_cast<int>(GameState::LEVEL_0));
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

        case GameState::GUIDE:
            gsIntialize = Guide::Initialize;
            gsUpdate = Guide::Update;
            gsDraw = Guide::Draw;
            gsFree = Guide::Free;
            break;

        case GameState::RESTART:
            next = previous;
            current = next;
            break;

        case GameState::EXIT:
            next = GameState::MAIN_MENU;
            break;
        }

        if (gsIntialize != nullptr) {
            gsIntialize();
        }

        SetAndPlayBGM();

        CameraManager::camera.Update(0.0f);
    }

    static void DebugCommands() {
        if (AEInputCheckTriggered(AEVK_TAB)) {
            Utils::isDebuggerOn = !Utils::isDebuggerOn;
            std::cout << std::string(Utils::isDebuggerOn ? "DEBUGGER ON\n" : "DEBUGGER OFF\n");
        }

        if (AEInputCheckTriggered(AEVK_M)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            next = GameState::MAIN_MENU;
        }
    }

    static void DebugUI() {
        if (!Utils::isDebuggerOn) return;

        UIManager::Panel debugPanel{ {200, 75}, {1.0f,1.0f,1.0f, 0.75f}, Utils::AnchorMode::BOTTOM_LEFT };
        debugPanel.Draw();

        TextObject debugText{ "", Utils::debuggerInfo.first * 0.75f, Utils::debuggerInfo.second, Transform{ {-1.f, -0.81f} , {0,0} } };
        debugText.TextContent("\n[TAB] Debugger ON\nFPS: " + Utils::FormatFloat(AEFrameRateControllerGetFrameRate(), 2)
            + "\nDeltaTime: " + Utils::FormatFloat(PhysicsManager::g_dt, 2)
            + "\n[M] Main Menu");
        debugText.Draw();
    }

    void SetAndPlayBGM() {
        switch (current) {
        case GameState::MAIN_MENU:
            AudioManager::PlayBGM(AudioManager::Track::MainMenu);
            break;

        case GameState::GUIDE:
            AudioManager::PlayBGM(AudioManager::Track::MapEditor);
            break;

        case GameState::MAP_EDITOR:
            AudioManager::PlayBGM(AudioManager::Track::MapEditor);
            break;

        case GameState::LEVEL_0:
        case GameState::LEVEL_1:
        case GameState::LEVEL_2:
        case GameState::LEVEL_3:
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
    }
}