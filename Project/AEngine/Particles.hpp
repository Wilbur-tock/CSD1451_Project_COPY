#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "AEEngine.h"
#include "Color.hpp"

// ============================================================================
// EmitParams
// ============================================================================
// Configuration bundle passed to ParticleManager::Emit().
// All "min/max" range fields pick a random value in [min, max] per particle.
// ============================================================================
class EmitParams
{
public:
    // ---- Spawn ----------------------------------------------------------
    AEVec2  spawnPosition{ 0.f, 0.f };   // World-space origin
    float   spawnRadius{ 0.f };         // Random offset radius from origin

    // ---- Motion ---------------------------------------------------------
    float   speedMin{ 50.f };
    float   speedMax{ 150.f };
    float   directionMin{ 0.f };         // Radians (0 = right)
    float   directionMax{ PI };          // Full circle by default

    float   rotationSpeedMin{ -PI };
    float   rotationSpeedMax{ PI };

    // ---- Appearance -----------------------------------------------------
    Color   colorStart{ 1.f, 1.f, 1.f, 1.f };
    Color   colorEnd{ 1.f, 1.f, 1.f, 0.f }; // Fade out alpha by default

    float   scaleStart{ 10.f };
    float   scaleEnd{ 0.f };

    // ---- Lifetime -------------------------------------------------------
    float   lifetimeMin{ 0.5f };
    float   lifetimeMax{ 1.5f };

    // ---- Count ----------------------------------------------------------
    int     count{ 10 };          // Particles to spawn per Emit() call
};

// ============================================================================
// Particle
// ============================================================================
// Represents a single particle instance.
// Managed exclusively by ParticleManager — do not create these directly.
// Holds an EmitParams as a member so its birth configuration is always
// accessible alongside its runtime state.
// ============================================================================
class Particle
{
public:
    // ---- Birth configuration --------------------------------------------
    EmitParams  emitParams{};     // The params this particle was born from

    // ---- Transform ----------------------------------------------------------
    AEVec2  position{};      // World-space position
    AEVec2  velocity{};      // Units per second
    float   rotation{ 0.f }; // Current rotation in radians
    float   rotationSpeed{ 0.f }; // Radians per second (can be negative)
    float   scaleStart{ 1.f }; // Uniform scale at birth
    float   scaleEnd{ 0.f }; // Uniform scale at death (lerped over lifetime)

    // ---- Appearance ---------------------------------------------------------
    Color   colorStart{ 1.f, 1.f, 1.f, 1.f }; // RGBA at birth
    Color   colorEnd{ 1.f, 1.f, 1.f, 0.f }; // RGBA at death (lerped)

    // ---- Lifetime -----------------------------------------------------------
    float   lifetimeMax{ 1.f };  // Total lifespan in seconds
    float   lifetimeCurrent{ 1.f };  // Seconds remaining
    bool    alive{ false };

    // ---- Helpers ------------------------------------------------------------

    // Normalized age: 0.0 = just born, 1.0 = about to die
    float NormalizedAge() const
    {
        if (lifetimeMax <= 0.f) return 1.f;
        return 1.f - (lifetimeCurrent / lifetimeMax);
    }
};

#endif // !PARTICLE_HPP