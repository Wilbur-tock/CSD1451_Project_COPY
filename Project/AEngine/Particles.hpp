#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "AEEngine.h"
#include "Color.hpp"

struct Particle
{
    AEVec2  position{};     // World-space position
    AEVec2  velocity{};     // Units per second
    float   rotation{ 0.f };// Current rotation in radians
    float   rotationSpeed{ 0.f };// Radians per second (can be negative)
    float   scaleStart{ 1.f };// Uniform scale at birth
    float   scaleEnd{ 0.f };// Uniform scale at death (lerped over lifetime)

    Color   colorStart{ 1.f, 1.f, 1.f, 1.f }; // RGBA at birth
    Color   colorEnd{ 1.f, 1.f, 1.f, 0.f }; // RGBA at death (lerped)

    float   lifetimeMax{ 1.f }; // Total lifespan in seconds
    float   lifetimeCurrent{ 1.f }; // Seconds remaining
    bool    alive{ false };


    // Normalized age: 0.0 = just born, 1.0 = about to die
    float NormalizedAge() const
    {
        if (lifetimeMax <= 0.f) return 1.f;
        return 1.f - (lifetimeCurrent / lifetimeMax);
    }
};

struct ParticleEmitParams
{
    AEVec2  spawnPosition{ 0.f, 0.f };   // World-space origin
    float   spawnRadius{ 0.f };         // Random offset radius from origin

    float   speedMin{ 50.f };
    float   speedMax{ 150.f };
    float   directionMin{ 0.f };         // Radians  (0 = right)
    float   directionMax{ PI };          // Full circle by default

    float   rotationSpeedMin{ -PI };
    float   rotationSpeedMax{ PI };

    Color   colorStart{ 1.f, 1.f, 1.f, 1.f };
    Color   colorEnd{ 1.f, 1.f, 1.f, 0.f }; // Fade out alpha by default

    float   scaleStart{ 10.f };
    float   scaleEnd{ 0.f };

    float   lifetimeMin{ 0.5f };
    float   lifetimeMax{ 1.5f };

    int     count{ 10 };          // Particles to spawn per Emit() call
};

#endif // !PARTICLE_HPP