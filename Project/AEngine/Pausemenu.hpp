/*
Authors: Nixon
Description: Display and manage functions about the pause menu page

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef PAUSEMENU_HPP
#define PAUSEMENU_HPP

#include "pch.hpp"
#include "Button.hpp"
#include "TextObject.hpp"

namespace PauseMenu {

    // Initialize buttons and text objects (call once after assets are loaded)
    void Init();

    // Draw the pause overlay and handle input.
    // Returns true while the menu is still active (game should stay paused).
    // Returns false when the player resumes or navigates away.
    void Update();

    // Draw the dimmed background, panel, title, and buttons
    void Draw();

    // Button callbacks
    void OnResumeClicked();
    void OnRestartClicked();
    void OnSettingsClicked();
    void OnBackToMenuClicked();

    // True after Init() has run at least once
    extern bool isInitialized;
}

#endif // !PAUSEMENU_HPP