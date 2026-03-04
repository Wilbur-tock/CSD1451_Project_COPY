#ifndef UI_INSTRUCTIONOVERLAY_HPP
#define UI_INSTRUCTIONOVERLAY_HPP

#include "pch.hpp"
#include "GameStateManager.hpp"

// This module draws the "Press [I]" instructions popup.
// The *toggle logic* (Utils::isInstructionOn) stays inside GameStateManager;
// this file is only responsible for drawing.

namespace UIInstructionsOverlay {
    void Draw(GameStateManager::GameState current);
}

#endif 