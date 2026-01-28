#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "pch.hpp"
#include "Button.hpp"

namespace MainMenu {
    // Game state functions
    void Load();
    void Initialize();
    void Update();
    void Draw();
    void Free();
    void Unload();

    // Button callback functions
    void OnPlayClicked();
    void OnMapEditorClicked();
    void OnQuitClicked();

    // Menu buttons
    extern Button playButton;
    extern Button mapEditorButton;
    extern Button quitButton;

    // Menu state
    extern bool isInitialized;
}

#endif // !MAINMENU_HPP