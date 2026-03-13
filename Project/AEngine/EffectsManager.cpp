#include "EffectsManager.hpp"
#include "ParticlesManager.hpp"

namespace EffectsManager {

    EffectsController effectsController;

    // Helper function
    void EffectsController::Emit(const EmitParams& params) {
        ParticleManager::particleController.Emit(params);
    }

    void EffectsController::PlayExplosion(AEVec2 position) {
        EmitParams params;

        params.spawnPosition = position;
        params.spawnRadius = 10.f;
        params.count = 40;

        params.speedMin = 80.f;
        params.speedMax = 250.f;
        params.directionMin = 0.f;
        params.directionMax = PI;       // Full 360 burst

        params.rotationSpeedMin = -PI;
        params.rotationSpeedMax = PI;

        params.colorStart = Color(1.f, 0.5f, 0.f, 1.f);  // Orange
        params.colorEnd = Color(0.8f, 0.1f, 0.f, 0.f); // Dark red, fade out

        params.scaleStart = 20.f;
        params.scaleEnd = 0.f;

        params.lifetimeMin = 0.3f;
        params.lifetimeMax = 0.7f;

        Emit(params);
        std::cout << "[EffectsManager] PlayExplosion at (" << position.x << ", " << position.y << ")\n";
    }

    void EffectsController::PlayTileBreak(AEVec2 position) {
        EmitParams params;

        params.spawnPosition = position;
        params.spawnRadius = 20.f;  // Spread across the tile's area
        params.count = 25;

        params.speedMin = 10.f;
        params.speedMax = 45.f;
        params.directionMin = 0.f;
        params.directionMax = PI;    // All directions

        params.rotationSpeedMin = -0.4f;
        params.rotationSpeedMax = 0.4f;

        params.colorStart = Color(0.75f, 0.70f, 0.65f, 0.85f); 
        params.colorEnd = Color(0.85f, 0.82f, 0.78f, 0.f);   

        params.scaleStart = 18.f;
        params.scaleEnd = 40.f;  

        params.lifetimeMin = 0.8f;
        params.lifetimeMax = 1.0f;  // 1 second

        Emit(params);
        std::cout << "[EffectsManager] PlayTileBreak at (" << position.x << ", " << position.y << ")\n";
    }

    void EffectsController::PlayProjectiileAbiltiesEffect(Effects effect, AEVec2 pos) {
        EmitParams particle;
        switch (effect) {
        case Effects::NUKE:
            particle = {
                pos,
                1,

                10,
                40,
                0.f,
                PI,

                -0.4f,
                0.4f,

                Color(1.0f, 0.0f, 0.0f),
                Color(1.0f, 0.0f, 0.0f, 0.f),
                0,
                32.0f * 50,
                0.8f,
                1.0f, 
                1,
            };
            std::cout << "[EffectsManager] NUKE at (" << pos.x << ", " << pos.y << ")\n";
            break;
        }

        ParticleManager::particleController.Emit(particle);
    }

} // namespace EffectsManager