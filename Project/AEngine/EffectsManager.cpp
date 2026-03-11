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

    void EffectsController::Emit(const ParticleEmitParams& params) {
        ParticleManager::particleController.Emit(params);
    }

} // namespace EffectsManager