#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "AEEngine.h"
#include "Color.hpp"

enum class ParticleShape { SQUARE, CIRCLE };

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

    // ---- Mesh ----------------------------------------------------------
    ParticleShape shape{ ParticleShape::SQUARE }; // Shape of the particle

    EmitParams() :
        spawnPosition{ 0.f, 0.f },
        spawnRadius{ 0.f },
        speedMin{ 50.f },
        speedMax{ 150.f },
        directionMin{ 0.f },
        directionMax{ PI },
        rotationSpeedMin{ -PI },
        rotationSpeedMax{ PI },
        colorStart{ 1.f, 1.f, 1.f, 1.f },
        colorEnd{ 1.f, 1.f, 1.f, 0.f },
        scaleStart{ 10.f },
        scaleEnd{ 0.f },
        lifetimeMin{ 0.5f },
        lifetimeMax{ 1.5f },
        count{ 10 },
        shape{ ParticleShape::SQUARE }
    {}

    EmitParams(
        AEVec2 spawnPosition, float spawnRadius,
        float speedMin, float speedMax,
        float directionMin, float directionMax,
        float rotationSpeedMin, float rotationSpeedMax,
        Color colorStart, Color colorEnd,
        float scaleStart, float scaleEnd,
        float lifetimeMin, float lifetimeMax,
        int count,
        ParticleShape shape = ParticleShape::SQUARE  // defaulted
    ) :
        spawnPosition{ spawnPosition }, spawnRadius{ spawnRadius },
        speedMin{ speedMin }, speedMax{ speedMax },
        directionMin{ directionMin }, directionMax{ directionMax },
        rotationSpeedMin{ rotationSpeedMin }, rotationSpeedMax{ rotationSpeedMax },
        colorStart{ colorStart }, colorEnd{ colorEnd },
        scaleStart{ scaleStart }, scaleEnd{ scaleEnd },
        lifetimeMin{ lifetimeMin }, lifetimeMax{ lifetimeMax },
        count{ count }, shape{ shape }
    {
    }
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