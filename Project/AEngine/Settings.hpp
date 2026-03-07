#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "pch.hpp"
#include "Button.hpp"
#include "TextObject.hpp"

namespace Settings {
    // Game state functions
    void Load();
    void Initialize();
    void Update();
    void Draw();
    void Free();
    void Unload();

    // Callbacks
    void OnBackClicked();
    void OnVolumeUpClicked();
    void OnVolumeDownClicked();

    // Volume state [0.0 .. 1.0]
    extern float currentVolume;

    extern Button backButton;
    extern Button volumeUpButton;
    extern Button volumeDownButton;
}

#endif // SETTINGS_HPP