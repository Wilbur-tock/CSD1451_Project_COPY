#include "EffectsManager.hpp"
#include "ParticlesManager.hpp"

namespace EffectsManager {

    EffectsController effectsController;

    // Helper function
    void EffectsController::Emit(const EmitParams& params) {
        ParticleManager::particleController.Emit(params);
    }

    void EffectsController::PlayExplosion(AEVec2 position) {
        Emit({
            position,   10.f,                               // spawn pos, radius
            80.f,       250.f,                              // speed min, max
            0.f,        PI,                                 // direction min, max
            -PI,        PI,                                 // rotation speed min, max
            Color(1.f, 0.5f, 0.f, 1.f),                   // color start (orange)
            Color(0.8f, 0.1f, 0.f, 0.f),                  // color end (dark red, fade)
            20.f,       0.f,                                // scale start, end
            0.3f,       0.7f,                               // lifetime min, max
            40,                                             // count
            ParticleShape::SQUARE
            });
        std::cout << "[EffectsManager] PlayExplosion at (" << position.x << ", " << position.y << ")\n";
    }

    void EffectsController::PlayTileBreak(AEVec2 position) {
        Emit({
            position,   20.f,                               // spawn pos, radius
            10.f,       45.f,                               // speed min, max
            0.f,        PI,                                 // direction min, max
            -0.4f,      0.4f,                               // rotation speed min, max
            Color(0.75f, 0.70f, 0.65f, 0.85f),            // color start
            Color(0.85f, 0.82f, 0.78f, 0.f),              // color end
            18.f,       40.f,                               // scale start, end
            0.8f,       1.0f,                               // lifetime min, max
            25,                                             // count
            ParticleShape::CIRCLE
            });
        std::cout << "[EffectsManager] PlayTileBreak at (" << position.x << ", " << position.y << ")\n";
    }

    void EffectsController::PlayProjectiileAbiltiesEffect(Effects effect, AEVec2 spawnPos) {
        // Default variables for an EXPLOSION effect
        float originOffset = 0;

        float speedMin{ 80.0f };
        float speedMax{ 250.f };

        float directionMin{ 0.f };
        float directionMax{ PI };       

        float rotationSpeedMin{ -PI };
        float rotationSpeedMax{ PI };

        Color colorStart{ 1.f, 0.5f, 0.f, 1.f };
        Color colorEnd{ 0.8f, 0.1f, 0.f, 0.f };

        float scaleStart{ 20.0f };
        float scaleEnd{ 0.0f };

        float lifetimeMin{ 0.5f };
        float lifetimeMax{ 1.5f };

        int   count{ 10 };
        ParticleShape shape{ ParticleShape::SQUARE };

        switch (effect) {
        case Effects::DEFAULT:
            originOffset = 10.0f;
            std::cout << "[EffectsManager] Play Explosion at (" << spawnPos.x << ", " << spawnPos.y << ")\n";
            break;
        case Effects::NUKE:
            colorStart = { 1.0f, 0, 0, 1.0f };

            speedMin = 0.0f;
            speedMax = 0.0f;

            scaleStart = 32.0f;
            scaleEnd = 32.0f * 15.0f;

            lifetimeMin = 3.0f;
            lifetimeMax = 5.0f;
            count = 1;
            shape = ParticleShape::CIRCLE;
            std::cout << "[EffectsManager] Play Nuke Explosion at (" << spawnPos.x << ", " << spawnPos.y << ")\n";
            break;
        }

        EmitParams particle = EmitParams{
            spawnPos,
            originOffset,

            speedMin,
            speedMax,
            directionMin,
            directionMax,

            rotationSpeedMin,
            rotationSpeedMax,

            colorStart,
            colorEnd,
            scaleStart,
            scaleEnd,

            lifetimeMin,
            lifetimeMax,

            count,
            shape
        };

        ParticleManager::particleController.Emit(particle);
    }

} // namespace EffectsManager