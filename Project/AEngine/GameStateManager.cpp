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

namespace GameStateManager {
    Utils::FuncPtr gsLoad, gsIntialize, gsUpdate, gsDraw, gsFree, gsUnload;
    GameState current, previous, next;
    bool setNewState = false;
    float loadingTimer = 0.0f;

    // Loading screen animation variables
    float dotAnimTimer = 0.0f;
    int dotCount = 0;

    TextObject debugText;
    TextObject instructionText;

    static std::string GetStateName(GameState s) {
        switch (s) {
        case GameState::INITIALIZE: return "INITIALIZE";
        case GameState::MAIN_MENU:  return "MAIN MENU";
        case GameState::MAP_EDITOR: return "MAP EDITOR";
        case GameState::LEVEL_1:    return "LEVEL 1";
        case GameState::LEVEL_2:    return "LEVEL 2";
        case GameState::CREDITS:    return "CREDITS";
        case GameState::RESTART:    return "RESTART";
        case GameState::EXIT:       return "EXIT";
        case GameState::QUIT:       return "QUIT";
        }
        return "GAME";
    }

    static void RenderInstructionsOverlay() {
        // Dim background
        UIManager::Panel dim{ {static_cast<float>(Config::SCREEN_WIDTH), static_cast<float>(Config::SCREEN_HEIGHT)},
            Color(0.0f, 0.0f, 0.0f, 0.55f), Utils::RectMode::RECT_CENTER };
        dim.Draw();

        // Main panel
        UIManager::Panel panel{ {1120.0f, 730.0f}, Color(0.12f, 0.12f, 0.16f, 0.95f), Utils::RectMode::RECT_CENTER };
        panel.Draw();

        // Build content based on current state
        std::string msg =
            "INSTRUCTIONS  (Press [I] to close)\n"
            "---------------------------------\n"
            "Current: " + GetStateName(current) + "\n\n"
            "GLOBAL\n"
            "[I] Toggle this popup\n"
            "[P] Toggle Debugger\n"
            "[M] Main Menu\n"
            "[ESC] Quit Game\n\n";

        if (current == GameState::LEVEL_1 || current == GameState::LEVEL_2) {
            msg +=
                "LEVEL\n"
                "Mouse: Aim\n"
                "Hold [LMB]: Charge shot (player squats)\n"
                "Release [LMB]: Shoot\n"
                "[0] [1] [2]: Change projectile ability\n\n"
                "NOTE: Game is paused while this popup is open.\n";
        }
        else if (current == GameState::MAP_EDITOR) {
            msg +=
                "MAP EDITOR\n"
                "[LMB]: Place tile\n"
                "[RMB]: Erase tile\n"
                "[CTRL]+Click: Fill COLUMN\n"
                "[SHIFT]+Click: Fill ROW\n"
                "Mouse Wheel: Change selected tile\n"
                "[T]: Toggle tile list\n"
                "[E]: Save\n"
                "[R]: Reset map\n"
                "[Q]: Quit editor\n"
                "[CTRL]+[1..5]: Load level file\n\n"
                "NOTE: Editor input is blocked while this popup is open.\n";
        }
        else if (current == GameState::MAIN_MENU) {
            msg +=
                "MAIN MENU\n"
                "[1] Play\n"
                "[2] Map Editor\n"
                "[3] Credits\n"
                "[Q] Quit\n";
        }

        // Position text near top-left of panel with padding
        float x = panel.Pos().x + 40.0f;
        float y = panel.Pos().y - 40.0f;
        Utils::NormalizeCoordinates(x, y);

        instructionText.textContent = msg;
        instructionText.textScale = 0.65f;
        instructionText.color = { 1.0f, 1.0f, 1.0f, 1.0f };
        instructionText.pos = { x, y };
        instructionText.Draw();
    }

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
        GraphicsManager::Load();
        LevelManager::Load();
        MapManager::Load();
        MainMenu::Load();
        Credits::Load();

        // Initialize camera controller
        CameraManager::camera.Init();
        CameraManager::camera.SetMoveSpeed(1500.0f);

        // Set world boundaries
        float halfW = Config::SCREEN_WIDTH / 2.0f;
        float halfH = Config::SCREEN_HEIGHT / 2.0f;
        CameraManager::camera.SetBounds(
            Config::WORLD_MIN_X,
            Config::WORLD_MAX_X,
            Config::WORLD_MIN_Y,
            Config::WORLD_MAX_Y
        );

        debugText = { "", Utils::debuggerInfo.first * 0.75f, Utils::debuggerInfo.second, {-1.f, -0.8f} };
        instructionText = { "", 0.65f, Color(1.0f, 1.0f, 1.0f, 1.0f), {0.0f, 0.0f} };

        // Reset transition vars
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
                    gsUpdate = MapEditor::Update;
                    gsDraw = nullptr;
                    gsFree = MapEditor::Exit;
                    gsUnload = nullptr;
                    break;

                case GameState::LEVEL_1:
                case GameState::LEVEL_2:
                    LevelManager::SelectLevel(static_cast<int>(current) - static_cast<int>(GameState::LEVEL_1));
                    gsLoad = LevelManager::LoadLevel;
                    gsIntialize = LevelManager::InitializeLevel;
                    gsUpdate = LevelManager::UpdateLevel;
                    gsDraw = nullptr;
                    gsFree = LevelManager::FreeLevel;
                    gsUnload = nullptr;
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
                    next = previous;
                    break;

                case GameState::EXIT:
                    next = GameState::MAIN_MENU;
                    break;

                default:
                    break;
                }

                // ✅ IMPORTANT: call gsLoad before gsInitialize
                if (gsLoad != nullptr) {
                    gsLoad();
                }

                if (gsIntialize != nullptr) {
                    gsIntialize();
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
            UIManager::Panel panel{ {250, 75}, {1.0f,1.0f,1.0f, 0.75f}, Utils::RectMode::RECT_BOTTOM_LEFT };
            panel.Draw();

            debugText.textContent = "\n[P] Debugger ON\nFPS: " + Utils::FormatFloat(AEFrameRateControllerGetFrameRate(), 2)
                + "\nDeltaTime: " + Utils::FormatFloat(rawDt, 2)
                + "\n[M] Main Menu" + "\nCamera State: " + CameraManager::camera.PrintState()
                + "\n Zoom: " + Utils::FormatFloat(CameraManager::camera.GetZoom(), 2)
                + "\n" + CameraManager::camera.PrintVisibleTiles();

            debugText.Draw();
        }

        // Instructions overlay (draw last so it appears on top)
        if (Utils::isInstructionOn) {
            RenderInstructionsOverlay();
            return; // block camera movement while popup is open
        }

        // Update camera
        CameraManager::camera.Update(PhysicsManager::g_dt);
    }

    void Exit() {
        MainMenu::Unload();
        AssetsManager::Unload();
        GraphicsManager::Unload();
        MapManager::Unload();
    }
}