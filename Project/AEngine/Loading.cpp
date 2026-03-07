#include "Loading.hpp"

#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "TextObject.hpp"
#include "Color.hpp"
#include "Config.hpp"

namespace Loading {

    float loadingTimer = 0.0f;
    float dotAnimTimer = 0.0f;
    int   dotCount = 0;

    float spriteAnimTimer = 0.0f;
    int   spriteFrame = 0;

    // -------------------------------------------------------------------------
    // PLACEHOLDER - replace this path with your actual stickman sprite sheet.
    // The sheet must be a horizontal strip of SPRITE_FRAME_COUNT (6) equal frames.
    // Example layout (6 frames, each 50x50, total image 300x50):
    //   [ frame0 | frame1 | frame2 | frame3 | frame4 | frame5 ]
    // -------------------------------------------------------------------------
    static const char* STICKMAN_SHEET_PATH = "Assets/stickman_run.png";

    static AEGfxTexture* gStickmanTex = nullptr;

    void Init(float duration) {
        loadingTimer = duration;
        dotAnimTimer = 0.0f;
        dotCount = 0;
        spriteAnimTimer = 0.0f;
        spriteFrame = 0;

        // Load sprite sheet once (AssetsManager caches it, so repeated calls are free)
        gStickmanTex = AssetsManager::GetTexture(STICKMAN_SHEET_PATH);
    }

    void Render() {
        // Reset camera so loading screen always renders at a known position
        AEGfxSetCamPosition(0.0f, 0.0f);

        // Dark background
        AEGfxSetBackgroundColor(0.1f, 0.1f, 0.15f);

        float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());

        // -- Progress ---------------------------------------------------------
        constexpr float maxTime = 0.9f;
        float progress = 1.0f - (loadingTimer / maxTime);
        if (progress > 1.0f) progress = 1.0f;
        if (progress < 0.0f) progress = 0.0f;

        // -- Animated dots - use raw frame time so this works even when g_dt is paused
        dotAnimTimer += dt;
        if (dotAnimTimer >= 0.3f) {
            dotAnimTimer = 0.0f;
            dotCount = (dotCount + 1) % 4;
        }

        std::string dots = "";
        for (int i = 0; i < dotCount; i++) dots += ".";

        TextObject loadingTextObj{ "LOADING" + dots, 1.5f, Color(1.0f, 1.0f, 1.0f, 1.0f), { -0.15f, 0.2f } };
        loadingTextObj.Draw();

        // -- Loading bar ------------------------------------------------------
        constexpr float barWidth = 600.0f;
        constexpr float barHeight = 40.0f;
        constexpr float barX = 0.0f;   // center of bar in screen X
        constexpr float barY = 15.0f;  // top-left Y anchor

        // Background
        AEMtx33 bgMat = GraphicsManager::CreateTransformMatrix(
            barX - (barWidth / 2.0f), barY, 0, barWidth, barHeight);
        GraphicsManager::DrawScreenOverlay(AssetsManager::squareTexture, bgMat, Color(0.2f, 0.2f, 0.2f, 1.0f));

        // Fill
        float progressWidth = barWidth * progress;
        if (progressWidth > 0.0f) {
            AEMtx33 fillMat = GraphicsManager::CreateTransformMatrix(
                barX - (barWidth / 2.0f), barY, 0, progressWidth, barHeight);
            GraphicsManager::DrawScreenOverlay(AssetsManager::squareTexture, fillMat, Color(0.3f, 0.7f, 1.0f, 1.0f));
        }

        // Percentage label
        int percentage = static_cast<int>(progress * 100);
        TextObject percentText{ std::to_string(percentage) + "%", 1.2f, Color(1.0f, 1.0f, 1.0f, 1.0f), { -0.03f, -0.15f } };
        percentText.Draw();

        // -- Stickman sprite animation ----------------------------------------
        // Advance frame at a fixed rate (independent of loading speed)
        spriteAnimTimer += dt;
        if (spriteAnimTimer >= SPRITE_FRAME_RATE) {
            spriteAnimTimer -= SPRITE_FRAME_RATE;
            spriteFrame = (spriteFrame + 1) % SPRITE_FRAME_COUNT;
        }

        // spriteCenterX/Y are the CENTER of the sprite quad (center mesh).
        float barLeft = barX - (barWidth / 2.0f);

        float spriteCenterX = barLeft + progressWidth;
        float spriteCenterY = barY - barHeight + (SPRITE_RENDER_H * 0.35f);

        // Clamp so the sprite does not overshoot the bar's right boundary (made it overshoot)
        float maxCenterX = barLeft + barWidth /*- (SPRITE_RENDER_W * 0.5f)*/;
        if (spriteCenterX > maxCenterX) spriteCenterX = maxCenterX;

        GraphicsManager::DrawSpriteFrame(gStickmanTex, spriteFrame,
            spriteCenterX, spriteCenterY,
            SPRITE_RENDER_W, SPRITE_RENDER_H);
    }

} // namespace Loading