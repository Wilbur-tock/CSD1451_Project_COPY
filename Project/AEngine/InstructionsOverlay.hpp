#ifndef UI_INSTRUCTIONSOVERLAY_HPP
#define UI_INSTRUCTIONSOVERLAY_HPP

#include "pch.hpp"

// This module draws the "Press [I]" instructions popup.
// The *toggle logic* (Utils::isInstructionOn) stays inside GameStateManager;
// this file is only responsible for drawing.

namespace UIInstructionsOverlay {
    void Draw();
}

#endif 