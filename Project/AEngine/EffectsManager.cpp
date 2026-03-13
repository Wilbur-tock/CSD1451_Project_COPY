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
				ParticleShape::CIRCLE
            };
            std::cout << "[EffectsManager] NUKE at (" << pos.x << ", " << pos.y << ")\n";
            break;
        }

        ParticleManager::particleController.Emit(particle);
    }

} // namespace EffectsManager