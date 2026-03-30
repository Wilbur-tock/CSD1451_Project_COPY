/*
Authors: Nixon
Description: Manage and draw the loading UI and animations

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef LOADING_HPP
#define LOADING_HPP

#include "pch.hpp"

namespace Loading {

    extern float loadingTimer;
    extern float dotAnimTimer;
    extern int   dotCount;

    // --- Sprite animation state ---
    extern float spriteAnimTimer;   // Accumulates dt to know when to advance a frame
    extern int   spriteFrame;       // Current frame index (0 – SPRITE_FRAME_COUNT-1)

    // Sprite sheet layout constants
    // Assumes a horizontal strip: all 6 frames in one row
    constexpr int   SPRITE_FRAME_COUNT = 6;
    constexpr float SPRITE_FRAME_WIDTH = 1.0f / SPRITE_FRAME_COUNT; // UV width per frame
    constexpr float SPRITE_FRAME_RATE = 0.08f;  // seconds per frame (8 fps)

    // Sprite render size in pixels (screen-space)
    constexpr float SPRITE_RENDER_W = 60.0f;
    constexpr float SPRITE_RENDER_H = 60.0f;

    /*!***********************************************************************
    \brief
        Reset all loading screen state variables. Call once before starting
        a new state transition.
    \param[in] duration
        Total duration (in seconds) the loading screen will be shown.
    *************************************************************************/
    void Init(float duration = 0.9f);

    /*!***********************************************************************
    \brief
        Render the loading screen for the current frame.
        Resets the camera, draws the background, animated "LOADING..." text,
        the progress bar, the percentage label, and the running stickman sprite.
        Must be called every frame while a state transition is in progress.
    *************************************************************************/
    void Render();

} // namespace Loading

#endif // LOADING_HPP