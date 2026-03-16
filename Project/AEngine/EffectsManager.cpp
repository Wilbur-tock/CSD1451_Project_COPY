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

    void EffectsController::PlayProjectiileAbiltiesEffect(int projectileMode, AEVec2 spawnPos) {
        // Default variables for an EXPLOSION effect
        float originOffset = 0;

        AEVec2 speed{ 80.0f , 250.0f };
        AEVec2 directional{ 0.0f, PI };
        AEVec2 rotation{ -PI, PI };

        std::pair<Color, Color> color = { { 1.f, 0.5f, 0.f, 1.f }, { 0.8f, 0.1f, 0.f, 0.f } };
        AEVec2 scale{ 20.0f, 0.0f };
        AEVec2 lifespan{ 0.5f, 1.5f };

        int count{ 10 };
        ParticleShape shape{ ParticleShape::SQUARE };

        switch (projectileMode) {
        case 1: // default
            originOffset = 10.0f;
            break;
        case 2: // bomb
            speed = { 200.0f, 500.0f };
            directional = { 0.0f, 2.0f * PI };
            rotation = { -2.0f * PI, 2.0f * PI };
            color.first = { 1.0f, 0.65f, 0.0f, 1.0f };
            color.second = { 0.4f, 0.5f, 0.0f, 0.0f };
            scale = { 16.0f, 24.0f };
            count = 40;
            shape = ParticleShape::CIRCLE;
            break;
        case 3: // nuke
            speed = { 500.0f, 1000.0f };
            color.first = { 1.0f, 0, 0, 1.0f };
            speed = { 0,0 };
            scale = { 32.0f, 32.0f * 15.0f };
            lifespan = { 3.0f, 5.0f };
            count = 1;
            shape = ParticleShape::CIRCLE;
            break;
        case 7: // Shield_buster
            originOffset = 20.0f;
            speed = { 400.0f, 800.0f };
            directional = { 0.0f, 2.0f * PI };
            rotation = { -4.0f * PI, 4.0f * PI };
            lifespan = { 0.15f, 0.4f };
            count = 50;
        }

        EmitParams particle = EmitParams{
            spawnPos,
            originOffset,

            speed.x,
            speed.y,
            directional.x,
            directional.y,

            rotation.x,
            rotation.y,

            color.first,
            color.second,
            scale.x,
            scale.y,
            
            lifespan.x,
            lifespan.y,

            count,
            shape
        };

        ParticleManager::particleController.Emit(particle);
    }

} // namespace EffectsManager