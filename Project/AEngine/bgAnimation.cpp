/*
Authors: Nixon
Description: Background animations for the main menu and game levels

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "BgAnimation.hpp"

#include "AssetsManager.hpp"
#include "GraphicsManager.hpp"
#include "Config.hpp"
#include "Color.hpp"
#include "Utils.hpp"

// ============================================================================
// Internal helpers shared by both subsystems
// ============================================================================
namespace {
    // Draws one full-screen texture layer as a seamlessly looping horizontal scroll,
    // pinned to the screen via DrawScreenOverlay regardless of camera state.
    //
    // The layer is drawn twice side-by-side so the right copy fills the gap
    // left by the left copy as it drifts off the left edge - no seam visible.
    //
    // scrollX - current pixel offset; wraps at screen width inside this function
    // color   - tint/alpha (alpha < 1 lets layers beneath show through)
    void DrawScrollingLayer(AEGfxTexture* tex, float scrollX, Color color) {
        if (!tex) return; // PNG not in Assets/ yet; skip silently

        const float W = static_cast<float>(Config::SCREEN_WIDTH);
        const float H = static_cast<float>(Config::SCREEN_HEIGHT);

        // fmodf keeps the offset in [0, W) so positions never drift to large
        // values during a long session on the menu or level screen
        float offset = fmodf(scrollX, W);

        // DrawScreenOverlay uses a top-left-corner origin mesh and adds the
        // current camera position internally, so the layer stays screen-fixed
        // even when the world camera pans or zooms during gameplay
        float topY = H * 0.5f;
        float leftX = -W * 0.5f - offset;

        GraphicsManager::DrawScreenOverlay(tex,
            GraphicsManager::CreateTransformMatrix(leftX, topY, W, H), color);
        GraphicsManager::DrawScreenOverlay(tex,
            GraphicsManager::CreateTransformMatrix(leftX + W, topY, W, H), color);
    }
}

// ============================================================================
// BgAnimation::Clouds
// ============================================================================
namespace BgAnimation::Clouds {

    static constexpr int   kLayers = 2;
    static constexpr float kSpeeds[kLayers] = { 25.0f, 70.0f }; // pixels/sec, far then near

    static AEGfxTexture* tex[kLayers] = {};
    static float         scrollX[kLayers] = {};

    void Load() {
        // PLACEHOLDERS - provide these PNGs in Assets/:
        //   bg_clouds_far.png  - 1600x900, transparent bg, small faint distant clouds
        //   bg_clouds_near.png - 1600x900, transparent bg, larger denser clouds
        tex[0] = AssetsManager::GetTexture("Assets/bg_clouds_far.png");
        tex[1] = AssetsManager::GetTexture("Assets/bg_clouds_near.png");
    }

    void Init() {
        // Zero out scroll offsets so re-entering a screen always starts cleanly.
        // Without this a level restart would resume mid-scroll from wherever it left off.
        for (int i = 0; i < kLayers; ++i)
            scrollX[i] = 0.0f;
    }

    void Update(float dt) {
        // Accumulate scroll each frame. fmodf inside DrawScrollingLayer clamps
        // the value on draw, so these can grow freely without overflow risk.
        for (int i = 0; i < kLayers; ++i)
            scrollX[i] += kSpeeds[i] * dt;
    }

    void Draw() {
        // Far layer: semi-transparent (alpha 0.75) so the sky base shows through
        DrawScrollingLayer(tex[0], scrollX[0], Color(1.0f, 1.0f, 1.0f, 0.75f));

        // Near layer: fully opaque and faster - occludes far layer, reinforces depth
        DrawScrollingLayer(tex[1], scrollX[1], Color(1.0f, 1.0f, 1.0f, 1.0f));
    }
}

// ============================================================================
// BgAnimation::Projectiles
// ============================================================================
namespace BgAnimation::Projectiles {

    static constexpr int   kMaxProjectiles = 8;
    // Downward acceleration in pixels/sec^2. Lower = floatier arc, higher = steep drop.
    static constexpr float kGravity = 240.0f;

    struct BgProjectile {
        float x = 0, y = 0;         // screen-space center (world origin = screen center)
        float vx = 0, vy = 0;       // velocity in px/s; vx always positive (left-to-right)
        float size = 0.0f;         // quad diameter in pixels
        Color color;
        float respawnTimer = 0.0f; // seconds until relaunch; 0 = currently active
        bool  active = true;
    };

    static BgProjectile  pool[kMaxProjectiles];
    static AEGfxTexture* cannonballTex = nullptr;

    // Randomly reinitialise one projectile at the left edge with a new arc.
    static void Reset(BgProjectile& p) {
        const float W = static_cast<float>(Config::SCREEN_WIDTH);
        const float H = static_cast<float>(Config::SCREEN_HEIGHT);

        // Start just off the left edge at a random height in the lower portion
        // of the screen so each arc peaks somewhere above mid-screen
        p.x = -W * 0.5f - Utils::RandomValue(20.0f, 140.0f);
        p.y = Utils::RandomValue(-H * 0.15f, H * 0.20f);

        // Rightward speed varies so projectiles have different flight times
        p.vx = Utils::RandomValue(500.0f, 650.0f);
        // Initial upward velocity; gravity curves the path downward during flight
        p.vy = Utils::RandomValue(120.0f, 320.0f);

        // Diameter: slight variation makes the pool feel less uniform
        p.size = Utils::RandomValue(14.0f, 26.0f);

        // Dark stone-gray tint, semi-transparent so they read as background
        // elements and never compete with foreground gameplay objects
        float shade = Utils::RandomValue(0.15f, 0.40f);
        p.color = Color(shade, shade * 0.95f, shade * 0.9f, 0.70f);

        p.respawnTimer = 0.0f;
        p.active = true;
    }

    void Load() {
        // PLACEHOLDER - provide Assets/bg_cannonball.png (~32x32, transparent bg).
        // Falls back to circleTexture (always loaded by AssetsManager) so projectiles
        // are visible immediately even without dedicated art.
        cannonballTex = AssetsManager::GetTexture("Assets/bg_cannonball.png");
        if (!cannonballTex) cannonballTex = AssetsManager::circleTexture;
    }

    void Init() {
        // Reset all projectiles and stagger them so they appear mid-arc at the
        // moment the screen first shows, rather than all launching from the left
        // edge simultaneously which would leave the screen empty until they cross it.
        //
        // Strategy: reset normally then simulate forward by a fraction of the
        // estimated full flight time (~4.5 s) using the same physics as Update().
        const float kSpread = 4.5f; // seconds spread across the full pool

        for (int i = 0; i < kMaxProjectiles; ++i) {
            Reset(pool[i]);

            // stagger[0] = 0 s (already at left edge), stagger[last] = kSpread s (near right edge)
            float stagger = (static_cast<float>(i) / static_cast<float>(kMaxProjectiles)) * kSpread;

            // Integrate forward: x is linear, y uses the kinematic equation
            // so the staggered position matches exactly where Update() would have put it
            pool[i].x += pool[i].vx * stagger;
            pool[i].y += pool[i].vy * stagger - 0.5f * kGravity * stagger * stagger;
            pool[i].vy -= kGravity * stagger; // update velocity to match the simulated time
        }
    }

    void Update(float dt) {
        const float W = static_cast<float>(Config::SCREEN_WIDTH);
        const float H = static_cast<float>(Config::SCREEN_HEIGHT);

        for (int i = 0; i < kMaxProjectiles; ++i) {
            BgProjectile& p = pool[i];

            if (!p.active) {
                // Waiting to respawn: count down then relaunch
                p.respawnTimer -= dt;
                if (p.respawnTimer <= 0.0f) Reset(p);
                continue;
            }

            // Semi-implicit Euler: update velocity first, then position.
            // This matches PhysicsManager's integration and gives a stable arc.
            p.vy -= kGravity * dt;
            p.x += p.vx * dt;
            p.y += p.vy * dt;

            // Deactivate once the projectile has fully left the visible screen.
            // vx is always positive so right-edge exit is the normal case;
            // bottom-edge exit handles a steep arc that drops below the screen.
            bool offRight = p.x > W * 0.5f + p.size;
            bool offBottom = p.y < -H * 0.5f - p.size;

            if (offRight || offBottom) {
                p.active = false;
                // Random delay prevents all respawns clustering at the same moment
                p.respawnTimer = Utils::RandomValue(0.3f, 3.5f);
            }
        }
    }

    void Draw() {
        for (int i = 0; i < kMaxProjectiles; ++i) {
            BgProjectile const& p = pool[i];
            if (!p.active) continue;

            // DrawScreenOverlay uses a top-left-corner-origin mesh.
            // To center the quad at (p.x, p.y) we offset by half the size:
            //   left edge  = p.x - half       (x increases rightward)
            //   top edge   = p.y + half        (y increases upward in world space)
            float half = p.size * 0.5f;
            GraphicsManager::DrawScreenOverlay(
                cannonballTex,
                GraphicsManager::CreateTransformMatrix(
                    p.x - half,
                    p.y + half,
                    p.size,
                    p.size),
                p.color);
        }
    }
}

// ============================================================================
// BgAnimation - combined helpers that call both subsystems
// ============================================================================
namespace BgAnimation {

    void Load() {
        Clouds::Load();
        Projectiles::Load();
    }

    void Init() {
        Clouds::Init();
        Projectiles::Init();
    }

    void Update(float dt) {
        Clouds::Update(dt);
        Projectiles::Update(dt);
    }

    void Draw() {
        Clouds::Draw();
        Projectiles::Draw();
    }
}