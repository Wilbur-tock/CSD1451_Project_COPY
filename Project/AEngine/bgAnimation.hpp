#ifndef BGANIMATION_HPP
#define BGANIMATION_HPP

// BgAnimation - shared animated background system used by MainMenu, LevelSelect, and Level.
//
// The two effects are fully independent. You can use clouds only, projectiles only, or both.
// All drawing uses DrawScreenOverlay so every layer stays screen-fixed regardless of
// whether the world camera is static (menus) or actively panning and zooming (levels).
//
// --- CLOUDS ---
//   Clouds::Load()        - load the two cloud sheet textures (safe to call multiple times)
//   Clouds::Init()        - reset scroll offsets to zero
//   Clouds::Update(dt)    - advance scroll offsets by one frame
//   Clouds::Draw()        - draw far + near cloud layers
//
// --- PROJECTILES ---
//   Projectiles::Load()        - load the cannonball texture (falls back to circleTexture)
//   Projectiles::Init()        - reset and stagger all projectiles across the screen
//   Projectiles::Update(dt)    - advance physics for one frame
//   Projectiles::Draw()        - draw all active projectiles
//
// --- COMBINED (both effects together) ---
//   BgAnimation::Load()        - calls Clouds::Load() + Projectiles::Load()
//   BgAnimation::Init()        - calls Clouds::Init() + Projectiles::Init()
//   BgAnimation::Update(dt)    - calls Clouds::Update(dt) + Projectiles::Update(dt)
//   BgAnimation::Draw()        - calls Clouds::Draw() + Projectiles::Draw()

namespace BgAnimation {

    // -----------------------------------------------------------------------
    // Clouds - two horizontally-scrolling parallax layers
    // -----------------------------------------------------------------------
    namespace Clouds {
        // Load cloud sheet textures from Assets/.
        // PLACEHOLDERS: bg_clouds_far.png and bg_clouds_near.png (1600x900, transparent bg).
        // If either file is missing the layer is silently skipped; nothing crashes.
        void Load();

        // Reset both scroll offsets to zero.
        // Call from Initialize() so re-entering a screen always starts cleanly.
        void Init();

        // Advance scroll offsets by dt seconds.
        // dt - elapsed time in seconds (AEFrameRateControllerGetFrameTime() or PhysicsManager::g_dt)
        void Update(float dt);

        // Draw far cloud layer (semi-transparent, slow) then near cloud layer (opaque, fast).
        // Call after the sky base and before any game content.
        void Draw();
    }

    // -----------------------------------------------------------------------
    // Projectiles - cannonballs that arc left-to-right in parabolic paths
    // -----------------------------------------------------------------------
    namespace Projectiles {
        // Load the cannonball texture from Assets/bg_cannonball.png.
        // PLACEHOLDER: ~32x32 circular stone sprite on a transparent background.
        // Falls back to AssetsManager::circleTexture automatically if the file is missing.
        void Load();

        // Reset all projectiles and stagger them across the screen so they appear
        // mid-arc when the screen first shows, rather than all launching from the left
        // edge at the same moment.
        void Init();

        // Advance projectile physics (gravity + velocity integration) by dt seconds.
        // Projectiles that leave the screen are deactivated and scheduled to respawn
        // after a random delay so the pool density stays roughly constant.
        void Update(float dt);

        // Draw all currently active projectiles.
        // Call after the sky base (and optionally after clouds) but before game content.
        void Draw();
    }

    // -----------------------------------------------------------------------
    // Combined helpers - call both subsystems in one go
    // -----------------------------------------------------------------------

    // Calls Clouds::Load() followed by Projectiles::Load().
    void Load();

    // Calls Clouds::Init() followed by Projectiles::Init().
    void Init();

    // Calls Clouds::Update(dt) followed by Projectiles::Update(dt).
    void Update(float dt);

    // Calls Clouds::Draw() followed by Projectiles::Draw().
    void Draw();
}

#endif // !BGANIMATION_HPP