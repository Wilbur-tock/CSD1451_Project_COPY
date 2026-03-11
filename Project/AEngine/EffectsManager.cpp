#include "EffectsManager.hpp"

namespace EffectsManager {

    EffectsController effectsController;

    void EffectsController::PlayExplosion(AEVec2 position) {
        ParticleEmitParams params;

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

    void EffectsController::PlayTileBreak(AEVec2 position, AEVec2 size) {
        ParticleEmitParams params;

        AEVec2 center = {
            position.x,
            position.y - size.y * 0.5f
        };

        params.spawnPosition = center;
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

    // Helper function
    void EffectsController::Emit(const ParticleEmitParams& params) {
        ParticleManager::particleController.Emit(params);
    }

} // namespace EffectsManager