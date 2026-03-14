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
            ParticleShape::SQUARE
            });
        std::cout << "[EffectsManager] PlayTileBreak at (" << position.x << ", " << position.y << ")\n";
    }

    void EffectsController::PlayProjectiileAbiltiesEffect(Effects effect, AEVec2 spawnPos) {
        float originOffset = 0;

        float speedMin{ 50.f };
        float speedMax{ 150.f };
        float directionMin{ 0.f };
        float directionMax{ PI };       

        float rotationSpeedMin{ -PI };
        float rotationSpeedMax{ PI };

        Color colorStart{ 1.f, 1.f, 1.f, 1.f };
        Color colorEnd{ 1.f, 1.f, 1.f, 0.f }; 

        float scaleStart{ 10.f };
        float scaleEnd{ 0.f };

        float lifetimeMin{ 0.5f };
        float lifetimeMax{ 1.5f };

        int   count{ 10 };
        ParticleShape shape{ ParticleShape::SQUARE };

        switch (effect) {
        case Effects::NUKE:
            colorStart = { 1.0f, 0, 0, 1.0f };
            colorStart = { 1.0f, 0, 0, 0.0f };
            scaleStart = 32.0f * 10;

            count = 1000;
            shape = ParticleShape::CIRCLE;
            std::cout << "[EffectsManager] Player Nuke Explosion at (" << spawnPos.x << ", " << spawnPos.y << ")\n";
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