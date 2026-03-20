#ifndef EFFECTSMANAGER_HPP
#define EFFECTSMANAGER_HPP

#include "AEEngine.h"
#include "Particles.hpp"

namespace EffectsManager {
    class EffectsController {
    public:
        void PlayExplosion(AEVec2 position);
        void PlayTileBreak(AEVec2 position);
		void PlaySmoke(AEVec2 position);
        void PlaySmokeCloud(AEVec2 position);
        void PlayProjectiileAbiltiesEffect(int projectileMode, AEVec2 pos);

    private:
        // Helper to fire off a fully configured params bundle
        void Emit(const EmitParams& params);
    };

    extern EffectsController effectsController;

} // namespace EffectsManager

#endif // !EFFECTSMANAGER_HPP