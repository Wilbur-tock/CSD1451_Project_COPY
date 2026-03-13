#ifndef EFFECTSMANAGER_HPP
#define EFFECTSMANAGER_HPP

#include "AEEngine.h"
#include "Particles.hpp"

namespace EffectsManager {
    enum class Effects {
        NUKE,
    };

    class EffectsController {
    public:
        void PlayExplosion(AEVec2 position);
        void PlayTileBreak(AEVec2 position);
        void PlayProjectiileAbiltiesEffect(Effects effect, AEVec2 pos);

    private:
        // Helper to fire off a fully configured params bundle
        void Emit(const EmitParams& params);
    };

    extern EffectsController effectsController;

} // namespace EffectsManager

#endif // !EFFECTSMANAGER_HPP