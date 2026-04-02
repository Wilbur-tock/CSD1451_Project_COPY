/*
Authors: Nixon
Description: DigiPen splash screen shown once on game launch.

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef SPLASH_HPP
#define SPLASH_HPP

#include "pch.hpp"

namespace Splash {

    // Total duration of the splash screen in seconds.
    // Split as: FADE_IN_DURATION + HOLD_DURATION + FADE_OUT_DURATION = TOTAL_DURATION
    constexpr float FADE_IN_DURATION = 0.7f;
    constexpr float HOLD_DURATION = 0.6f;
    constexpr float FADE_OUT_DURATION = 0.7f;
    constexpr float TOTAL_DURATION = FADE_IN_DURATION + HOLD_DURATION + FADE_OUT_DURATION; // 2.0 s

    // Game-state lifecycle functions (assigned to GameStateManager function pointers)
    void Initialize();
    void Update();
    void Draw();
    void Free();

} // namespace Splash

#endif // SPLASH_HPP