#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "pch.hpp"
#include "Button.hpp"
#include "TextObject.hpp"

namespace Settings {
    void Load();
    void Initialize();
    void Update();
    void Draw();
    void Free();
    void Unload();

    void OnBackClicked();

    void OnMasterVolumeUpClicked();
    void OnMasterVolumeDownClicked();

    void OnBGMVolumeUpClicked();
    void OnBGMVolumeDownClicked();

    void OnSFXVolumeUpClicked();
    void OnSFXVolumeDownClicked();

    extern float currentMasterVolume;
    extern float currentBgmVolume;
    extern float currentSfxVolume;

    extern Button backButton;

    extern Button masterVolumeUpButton;
    extern Button masterVolumeDownButton;

    extern Button bgmVolumeUpButton;
    extern Button bgmVolumeDownButton;

    extern Button sfxVolumeUpButton;
    extern Button sfxVolumeDownButton;
}

#endif // SETTINGS_HPP