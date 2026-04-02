/*
Authors: Nixon
Description: DigiPen splash screen shown once on game launch.

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "Splash.hpp"
#include "AssetsManager.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "CameraManager.hpp"
#include "UIManager.hpp"
#include "Config.hpp"
#include "Color.hpp"
#include "AEEngine.h"
#include <fstream>

namespace Splash {

    namespace {
        // Path to the sentinel file that records whether the splash has
        // been shown before. Stored alongside the audio settings so it
        // follows the same persistence convention as the rest of the project.
        constexpr const char* SPLASH_FLAG_PATH = "Assets/Data/splashShown.txt";

        float splashTimer = 0.0f;           // Seconds elapsed since the splash started
        AEGfxTexture* logoTexture = nullptr; // DigiPen logo

        // Compute the logo alpha for the current timer value.
        // Returns a value in [0, 1] following the three-phase ramp described above.
        float ComputeAlpha(float timer) {
            const float holdEnd = FADE_IN_DURATION + HOLD_DURATION;

            if (timer < FADE_IN_DURATION) {
                // Phase 1: fade in -- alpha rises linearly from 0 to 1
                return timer / FADE_IN_DURATION;
            }
            else if (timer < holdEnd) {
                // Phase 2: hold -- logo fully opaque
                return 1.0f;
            }
            else {
                // Phase 3: fade out -- alpha falls linearly from 1 to 0
                float fadeOutProgress = (timer - holdEnd) / FADE_OUT_DURATION;
                return 1.0f - fadeOutProgress;
            }
        }
    }

    // ---------------------------------------------------------------------------
    void Initialize() {
        // Check whether the splash has already been shown on a previous launch.
        // If the sentinel file exists, skip the splash entirely this session by
        // transitioning to MAIN_MENU immediately. Update() will see the same
        // condition and also skip, so no frames of the splash are drawn.
        std::ifstream flagCheck(SPLASH_FLAG_PATH);
        if (flagCheck.good()) {
            GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
            return; // skip all remaining setup -- Update() will handle the transition
        }

        splashTimer = 0.0f;

        // Cache the DigiPen logo. AssetsManager owns the texture lifetime.
        logoTexture = AssetsManager::GetTexture("Assets/DigiPen_Logo.png");

        // Lock the camera at the world origin so DrawScreenOverlay coordinates
        // map predictably. The main menu does the same thing.
        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);
    }

    // ---------------------------------------------------------------------------
    void Draw() {
        // -- Layer 1: pure black background --
        // Drawn at full opacity every frame so the screen is never anything other
        // than black while the logo is transparent.
        UIManager::Panel blackBg{
            { static_cast<float>(Config::SCREEN_WIDTH),
              static_cast<float>(Config::SCREEN_HEIGHT) },
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            Utils::AnchorMode::CENTER
        };
        blackBg.Draw();

        // -- Layer 2: DigiPen logo at current fade alpha --
        if (!logoTexture) return;

        const float alpha = ComputeAlpha(splashTimer);

        // The texture is a full-frame 16:9 image (logo + copyright already
        // composited). Stretch it to cover the entire screen so it fills edge
        // to edge with no letterboxing. CreateTransformMatrix takes the
        // top-left corner, so X = -halfWidth and Y = +halfHeight (Y is up).
        const float w = static_cast<float>(Config::SCREEN_WIDTH);
        const float h = static_cast<float>(Config::SCREEN_HEIGHT);

        GraphicsManager::DrawScreenOverlay(
            logoTexture,
            GraphicsManager::CreateTransformMatrix(
                -w * 0.5f,  // left edge
                h * 0.5f,  // top edge
                w, h
            ),
            Color(1.0f, 1.0f, 1.0f, alpha) // white tint; alpha drives the fade
        );
    }

    // ---------------------------------------------------------------------------
    void Update() {
        // If Initialize() detected the sentinel file and already set next =
        // MAIN_MENU, logoTexture is null and splashTimer was never started.
        // Skip all update logic; the game loop will transition this frame.
        if (!logoTexture) return;

        // Advance timer using raw frame time -- not g_dt which is zeroed when paused.
        const float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());
        splashTimer += dt;

        // Clamp so ComputeAlpha never receives a value past TOTAL_DURATION
        // (avoids a negative alpha on the very last frame before transition).
        if (splashTimer > TOTAL_DURATION) {
            splashTimer = TOTAL_DURATION;
        }

        // Render the current frame
        Draw();

        // Once the full two-second window has elapsed, move to the main menu.
        // Writing to GameStateManager::next does not switch states immediately;
        // the game loop picks it up at the end of this frame.
        if (splashTimer >= TOTAL_DURATION) {
            GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
        }
    }

    // ---------------------------------------------------------------------------
    void Free() {
        // Write the sentinel file so the splash is skipped on every subsequent
        // launch. The file only needs to exist -- its contents do not matter.
        // Using the same Assets/Data/ directory as the audio settings so all
        // persistent data lives in one place.
        //
        // This is called after the full fade-out completes, so a player who
        // force-quits mid-splash will see it again next launch (the file is
        // not written until the splash finishes naturally).
        std::ofstream flag(SPLASH_FLAG_PATH, std::ios::trunc);

        // logoTexture is owned by AssetsManager -- do not free it here.
        // splashTimer does not need to be reset; Initialize() resets it if
        // the state is ever re-entered (which will not normally happen after
        // the file is written).
    }

} // namespace Splash