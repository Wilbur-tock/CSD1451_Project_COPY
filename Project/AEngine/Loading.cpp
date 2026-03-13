#include "Loading.hpp"

#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Color.hpp"
#include "UIManager.hpp"
#include "Config.hpp"
#include "Utils.hpp"

namespace Loading {
    // State variables
    float loadingTimer = 0.0f;

    float dotAnimTimer = 0.0f;
    int   dotCount = 0;

    float spriteAnimTimer = 0.0f;
    int   spriteFrame = 0;


    // The sheet must be a horizontal strip of SPRITE_FRAME_COUNT (6) equal frames.
    // Example layout (6 frames, each 50x50, total image 300x50):
    // [ frame0 | frame1 | frame2 | frame3 | frame4 | frame5 ]
    static const char* STICKMAN_SHEET_PATH = "Assets/stickman_run.png"; // Path to the stickman sprite sheet

    static AEGfxTexture* gStickmanTex = nullptr; // Pointer to the loaded stickman texture

    // UI Objects
    TextObject loadingTextObj, percentageText;
    UIManager::Bar loadingBar;

    void Init(float duration) {
        loadingTimer = duration; // Set the countdown timer to the requested duration (defined in .hpp)
        dotAnimTimer = 0.0f;
        dotCount = 0;
        spriteAnimTimer = 0.0f;
        spriteFrame = 0;

        // Load sprite sheet once (AssetsManager caches it, so repeated calls are free)
        gStickmanTex = AssetsManager::GetTexture(STICKMAN_SHEET_PATH);

        // Initialize UI Objects
        loadingTextObj = TextObject{
            "",
            1.5f,
            Color(1.0f, 1.0f, 1.0f, 1.0f),
            { -0.15f, 0.2f }
        };

        percentageText = TextObject{ "",
            1.2f,
            Color(1.0f, 1.0f, 1.0f, 1.0f),
            { -0.03f, -0.15f }
        };

        Transform loadingBarTrans{ {0,0}, {600.0f, 40.0f} };
        Utils::SetAnchoredPosition(loadingBarTrans, Utils::AnchorMode::CENTER);
        loadingBar = UIManager::Bar{
            loadingBarTrans.pos, loadingBarTrans.size, 
            Color{0.3f, 0.7f, 1.0f},
            Color{0.2f, 0.2f, 0.2f}
        };
    }

    void Render() {
        // Reset camera so loading screen always renders at a known position
        AEGfxSetCamPosition(0.0f, 0.0f);

        // Dark background
        AEGfxSetBackgroundColor(0.1f, 0.1f, 0.15f);

        // Read the real elapsed frame time for this frame
        // Runs even when game is frozen
        float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());

        // Progress calcuation
        // maxTime must match the default duration passed to Init()
        // so progress reaches exactly 1.0f when timer hits 0
        constexpr float maxTime = 0.9f;

        float progress = 1.0f - (loadingTimer / maxTime); // Derive a 0.0f -> 1.0f progress fraction from the remaining timer

        // Clamp to [0.0, 1.0] to handle floating-point overshoot in either direction
        if (progress > 1.0f) progress = 1.0f;
        if (progress < 0.0f) progress = 0.0f;

        // -- Animated dots - use raw frame time so this works even when g_dt is paused
        dotAnimTimer += dt;
        if (dotAnimTimer >= 0.3f) {          // Every 0.3 seconds (3.3 times per second), advance the dot count
            dotAnimTimer = 0.0f;             // Reset the accumulator

            dotCount = (dotCount + 1) % 4;   // Cycle: 0 -> 1 -> 2 -> 3 -> 0 -> ...
        }

        // Build the dot string by appending dotCount '.' characters
        std::string dots = "";      // Start with an empty string
        for (int i = 0; i < dotCount; i++) dots += "."; // Each iteration appends one dot
                                                        // Result: "", ".", "..", "..." cycling every 0.3s
        loadingTextObj.TextContent("LOADING" + dots);
        loadingTextObj.Draw();  // Submit the text to the renderer

        // Loading bar
        //constexpr float barWidth = 600.0f;
        //constexpr float barHeight = 40.0f;
        //constexpr float barX = 0.0f;   // center of bar in screen X
        //constexpr float barY = 15.0f;  // top-left Y anchor
        //
        //// Loading Bar background (grey track) --
        //AEMtx33 bgMat = GraphicsManager::CreateTransformMatrix(
        //    barX - (barWidth / 2.0f), // Left edge X of the bar
        //    barY,                     // Top edge Y of the bar
        //    0,                        // No rotation
        //    barWidth,                 // Full width of the grey background track
        //    barHeight                 // Full height
        //);
        //// Draw the grey background quad using the shared white square texture tinted dark grey
        //// Avoids needing a separate grey texture asset for a bar background
        //GraphicsManager::DrawScreenOverlay(AssetsManager::squareTexture, bgMat, Color(0.2f, 0.2f, 0.2f, 1.0f));
        ////Loading Bar fill (blue progress) --
        //float progressWidth = barWidth * progress; // Scale the full bar width by progress (0.0 to 1.0)
        //if (progressWidth > 0.0f) {                // Skip drawing the fill entirely when progress is 0 to avoid rendering a 0-width quad
        //    AEMtx33 fillMat = GraphicsManager::CreateTransformMatrix(
        //        barX - (barWidth / 2.0f), 
        //        barY, 
        //        0, 
        //        progressWidth, 
        //        barHeight
        //    );
        //    GraphicsManager::DrawScreenOverlay(AssetsManager::squareTexture, fillMat, Color(0.3f, 0.7f, 1.0f, 1.0f));
        //}

        float barWidth  = loadingBar.GetTransform().size.x;
        float barHeight = loadingBar.GetTransform().size.y;
        float barX      = loadingBar.GetTransform().pos.x; 
        float barY      = loadingBar.GetTransform().pos.y;

        float progressWidth = barWidth * progress; // Scale the full bar width by progress (0.0 to 1.0)
        loadingBar.Draw(progressWidth, barWidth);


        // Percentage label
        int percentage = static_cast<int>(progress * 100); //// Convert progress to an integer percentage (0-100) for display
        percentageText.TextContent(std::to_string(percentage) + "%");
        percentageText.Draw();

        // Stickman sprite animation
        // Advance frame at a fixed rate (independent of loading speed)
        spriteAnimTimer += dt;
        if (spriteAnimTimer >= SPRITE_FRAME_RATE) { // SPRITE_FRAME_RATE is the number of seconds per frame
                                                    // When the accumulator reaches or exceeds one frame's duration, advance
            spriteAnimTimer -= SPRITE_FRAME_RATE;   // Subtract rather than reset to 0 so any overshoot carries over to the
                                                    // next frame, keeping the animation tempo stable regardless of frame rate
            // Advance to the next frame and wrap back to 0 after the last frame
            spriteFrame = (spriteFrame + 1) % SPRITE_FRAME_COUNT; // SPRITE_FRAME_COUNT = 6, so frames cycle 0->1->2->3->4->5->0->...
        }

        // spriteCenterX/Y are the CENTER of the sprite quad (center mesh).
        float barLeft = barX;

        // Place the sprite's center X at the leading edge of the fill (barLeft + progressWidth)
        // This makes the stickman appear to be running at the tip of the blue fill bar
        float spriteCenterX = barLeft + progressWidth;
        // Place the sprite's center Y just above the top of the bar
        float spriteCenterY = barY - barHeight + (SPRITE_RENDER_H * 0.35f);

        // Clamp so the sprite does not overshoot the bar's right boundary (made it overshoot)
        float maxCenterX = barLeft + barWidth /*- (SPRITE_RENDER_W * 0.5f)*/;
        if (spriteCenterX > maxCenterX) spriteCenterX = maxCenterX;

        // DrawSpriteFrame calculates the UV offset for spriteFrame internally and
        // applies the camera offset correction so the sprite stays in screen space.
        GraphicsManager::DrawSpriteFrame(
            gStickmanTex,   // The sprite sheet texture (all frames in one image).
            spriteFrame,    // Which frame column to show this frame.
            spriteCenterX,  // Horizontal center of the sprite quad in screen space.
            spriteCenterY,  // Vertical center of the sprite quad in screen space.
            SPRITE_RENDER_W, // Rendered width in screen units.
            SPRITE_RENDER_H  // Rendered height in screen units.
        );
    }

} // namespace Loading