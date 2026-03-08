#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "pch.hpp"
#include "Button.hpp"

namespace MainMenu {
    extern float titleAnimTimer;
    extern float baseTitleScale;
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
    void OnCreditsClicked();
    void OnSettingsClicked();
    void OnQuitClicked();

    // Menu buttons
    extern Button playButton;
    extern Button mapEditorButton;
    extern Button creditsButton;
    extern Button settingsButton;
    extern Button quitButton;

    // Menu state
    extern bool isInitialized;
}

#endif // !MAINMENU_HPP