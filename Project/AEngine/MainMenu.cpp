/*
Authors: Nixon
Description: Display and manage functions about the main menu page

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "MainMenu.hpp"
#include "ConfirmDialog.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"
#include "Utils.hpp"
#include "AudioManager.hpp"
#include "CameraManager.hpp"
#include "bgAnimation.hpp"

namespace MainMenu {
    // Menu buttons
    Button playButton, mapEditorButton, creditsButton, settingsButton, guideButton, quitButton;
    TextObject gameTitle, instructions;

    // Menu state
    bool isInitialized = false;

    // Background texture
    AEGfxTexture* bgTexture = nullptr;     // static sky base layer
    AEGfxTexture* buttonTexture = nullptr;

    // Game title animation
    float titleAnimTimer = 0.0f;
    float baseTitleScale = 2.5f;

    void Load() {
        // Load textures that persist for the lifetime of the main menu state.
        // AssetsManager owns the texture lifetime; do not free them here.
        buttonTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");

        // Static sky base - drawn first, covers the entire screen
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");

        // Load shared cloud and cannonball textures for the animated background
        BgAnimation::Load();
    }

    void Initialize() {
        // Initialize menu elements
        isInitialized = true;

        // Button dimensions
        float btnWidth = 300.0f;  // Width of every button in world units.
        float btnHeight = 80.0f;   // Height of every button in world units.
        float btnSpacing = 100.0f;  // Vertical gap between button tops.

        // Calculate centered positions
        float centerX = -550.0f; // Center of screen in world coordinates
        float startY = 230.0f;  // Starting Y position for first button

        // Create Play Button
        // Transitions to LEVEL_SELECT. Blue tint.
        AEVec2 playPos = { centerX - btnWidth / 2.0f, startY };
        playButton = Button(
            TextObject{ "PLAY" },
            Utils::AnchorMode::CENTER,
            playPos,
            { btnWidth, btnHeight },
            Color(0.2f, 0.6f, 0.9f, 1.0f),
            buttonTexture,
            nullptr,
            OnPlayClicked
        );

        // Create Map Editor Button
        // Transitions to MAP_EDITOR. Green tint.
        AEVec2 editorPos = { centerX - btnWidth / 2.0f, startY - btnSpacing };
        mapEditorButton = Button(
            TextObject{ "MAP EDITOR" },
            Utils::AnchorMode::CENTER,
            editorPos,
            { btnWidth, btnHeight },
            Color(0.3f, 0.7f, 0.3f, 1.0f),
            buttonTexture,
            nullptr,
            OnMapEditorClicked
        );

        // Create Settings Button
        // Transitions to SETTINGS. Yellow tint.
        AEVec2 settingsPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 2 };
        settingsButton = Button(
            TextObject{ "SETTINGS" },
            Utils::AnchorMode::CENTER,
            settingsPos,
            { btnWidth, btnHeight },
            Color(0.6f, 0.6f, 0.2f, 1.0f),
            buttonTexture,
            nullptr,
            OnSettingsClicked
        );

        // Create Guide Button
        // Transitions to GUIDE. Orange tint.
        AEVec2 guidePos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 3 };
        guideButton = Button(
            TextObject{ "GUIDE" },
            Utils::AnchorMode::CENTER,
            guidePos,
            { btnWidth, btnHeight },
            Color(0.9f, 0.55f, 0.2f, 1.0f),
            buttonTexture,
            nullptr,
            OnGuideClicked
        );

        // Create Credits Button
        // Transitions to CREDITS. Purple tint.
        AEVec2 creditsPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 4 };
        creditsButton = Button(
            TextObject{ "CREDITS" },
            Utils::AnchorMode::CENTER,
            creditsPos,
            { btnWidth, btnHeight },
            Color(0.7f, 0.5f, 0.9f, 1.0f),
            buttonTexture,
            nullptr,
            OnCreditsClicked
        );

        // Create Quit Button
        // Exits the application. Red tint -- matches the danger color convention.
        AEVec2 quitPos = { centerX - btnWidth / 2.0f, startY - btnSpacing * 5 };
        quitButton = Button(
            TextObject{ "QUIT" },
            Utils::AnchorMode::CENTER,
            quitPos,
            { btnWidth, btnHeight },
            Color(0.8f, 0.2f, 0.2f, 1.0f),
            buttonTexture,
            nullptr,
            OnQuitClicked
        );

        // Game title text
        gameTitle = TextObject{ "FORTRESS FIGHT",
            1.0f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { 0.05f, 0.8f },
        };
        gameTitle.SetFont(AssetsManager::titleFont);

        // Keyboard shortcut hint at the bottom of the screen
        instructions = {
            "Click a button or use keyboard shortcuts:\n1 - Play | 2 - Map Editor | 3 - Settings | 4 - Guide | 5 - Credits | Q - Quit",
            0.7f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { -0.98f, -0.9f },
        };

        // Reset camera to origin and lock it so UI world-space coordinates map
        // directly to screen positions.
        CameraManager::camera.SetPosition({ 0, 0 });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        // Reset and stagger the animated background (clouds + projectiles)
        BgAnimation::Init();
    }

    void Draw() {
        // Layer 0: static sky base -- drawn first so clouds and projectiles sit on top
        if (bgTexture != nullptr) {
            GraphicsManager::DrawWorldUI(
                bgTexture,
                GraphicsManager::CreateTransformMatrix(-Config::SCREEN_WIDTH / 2.0f, Config::SCREEN_HEIGHT / 2.0f,
                    Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT),
                Color(1.0f, 1.0f, 1.0f, 1.0f)
            );
        }
        else {
            AEGfxSetBackgroundColor(0.1f, 0.1f, 0.15f);
        }

        // Animated background: scrolling clouds + arcing cannonball projectiles
        BgAnimation::Draw();

        // Draw title
        gameTitle.Draw();

        // Draw subtitle / instructions
        //instructions.Draw();
    }

    void Update() {
        float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());

        // -------------------------------------------------------------------
        // Freeze all time-driven animation while the confirm dialog is open.
        //
        // Clouds, projectiles, and the title pulse all advance using dt.
        // Passing zero instead of the real delta time keeps every animated
        // value exactly where it was the frame the dialog appeared, giving
        // the impression that the menu is paused in the background.
        // The draw call still runs every frame so the frozen state is visible
        // behind the dialog overlay.
        // -------------------------------------------------------------------
        const float animDt = ConfirmDialog::IsOpen() ? 0.0f : dt;

        // Advance animation timer (drives title pulse + flash)
        titleAnimTimer += animDt;

        // Advance cloud scroll and projectile physics for this frame
        BgAnimation::Update(animDt);

        // Pulse: scale oscillates +/- 0.2 around the base scale
        float pulseScale = baseTitleScale + 0.2f * sinf(titleAnimTimer * 2.0f);
        gameTitle.TextScale(pulseScale);

        // Flash: alpha oscillates between 0.5 and 1.0
        float flashAlpha = 0.75f + 0.25f * sinf(titleAnimTimer * 3.0f);
        gameTitle.SetColor(Color{
            gameTitle.GetColor().Red(),
            gameTitle.GetColor().Green(),
            gameTitle.GetColor().Blue(),
            flashAlpha
            });

        // -------------------------------------------------------------------
        // Keyboard shortcuts
        //
        // All shortcuts are blocked while the confirm dialog is open so the
        // player cannot accidentally trigger a navigation while answering it.
        // The Q / Quit shortcut routes through ConfirmDialog (same as the button).
        // -------------------------------------------------------------------
        if (!ConfirmDialog::IsOpen()) {
            if (AEInputCheckTriggered(AEVK_1)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnPlayClicked();
            }
            if (AEInputCheckTriggered(AEVK_2)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnMapEditorClicked();
            }
            if (AEInputCheckTriggered(AEVK_3)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnSettingsClicked();
            }
            if (AEInputCheckTriggered(AEVK_4)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnGuideClicked();
            }
            if (AEInputCheckTriggered(AEVK_5)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnCreditsClicked();
            }
            if (AEInputCheckTriggered(AEVK_Q)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnQuitClicked();
            }
        }

        Draw(); // Render frozen (or live) background, title, and instructions

        // -------------------------------------------------------------------
        // Confirm dialog intercept
        //
        // If the dialog is open, draw it on top of the frozen menu and skip
        // all button updates so clicks cannot reach the buttons behind it.
        // -------------------------------------------------------------------
        if (ConfirmDialog::IsOpen()) {
            ConfirmDialog::Update();
            return;
        }

        // Normal button updates (only reached when no confirm dialog is open)
        playButton.Update();
        mapEditorButton.Update();
        settingsButton.Update();
        guideButton.Update();
        creditsButton.Update();
        quitButton.Update();
    }

    void Free() {
        isInitialized = false;
    }

    void Unload() {
        // Textures are owned by AssetsManager; do not free them here.
    }

    // -------------------------------------------------------------------
    // Button callback functions
    //
    // Writing to GameStateManager::next does not switch states immediately.
    // The game loop reads next at the end of the frame and performs the
    // transition then.
    // -------------------------------------------------------------------

    void OnPlayClicked() {
        GameStateManager::next = GameStateManager::GameState::LEVEL_SELECT;
    }

    void OnMapEditorClicked() {
        GameStateManager::next = GameStateManager::GameState::MAP_EDITOR;
    }

    void OnSettingsClicked() {
        GameStateManager::next = GameStateManager::GameState::SETTINGS;
    }

    void OnGuideClicked() {
        GameStateManager::next = GameStateManager::GameState::GUIDE;
    }

    void OnCreditsClicked() {
        GameStateManager::next = GameStateManager::GameState::CREDITS;
    }

    void OnQuitClicked() {
        // Quitting closes the application -- show confirmation so the player
        // cannot accidentally exit from a misclick.
        ConfirmDialog::Open(
            "The game will close.",
            "Are you sure you want to quit?",
            []() {
                GameStateManager::next = GameStateManager::GameState::QUIT;
            }
        );
    }

} // namespace MainMenu