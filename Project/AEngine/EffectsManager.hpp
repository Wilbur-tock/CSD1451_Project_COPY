#ifndef EFFECTSMANAGER_HPP
#define EFFECTSMANAGER_HPP

#include "pch.hpp"
#include "ParticlesManager.hpp"


namespace EffectsManager {

    class EffectsController {
    public:
        void PlayExplosion(AEVec2 position);
        void PlayTileBreak(AEVec2 position, AEVec2 size);

    private:
        // Helper to fire off a fully configured params bundle
        void Emit(const ParticleEmitParams& params);
    };

    extern EffectsController effectsController;

} // namespace EffectsManager

#endif // !EFFECTSMANAGER_HPP