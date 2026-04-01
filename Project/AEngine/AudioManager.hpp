/*
Authors: Bryan
Description: Controls and play the audio

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef AUDIO_MANAGER_HPP
#define AUDIO_MANAGER_HPP

#include "pch.hpp"

namespace AudioManager {
    enum class Track {
        None,
        MainMenu,
        MapEditor,
        Level,
        Credits
    };

    enum class SFX {
        UIButton,
        CannonFire,
        Impact,
        TurnSwitch
    };

    void Load();
    void Update();
    void Shutdown();

    void LoadSettings();
    void SaveSettings();

    void PlayBGM(Track t);
    void StopBGM();
    void PlaySFX(SFX sfx);

    void PlayInstantAbilitySFX(const std::string& abilityName);
    void PlayLaunchAbilitySFX(const std::string& abilityName);

    // Master volume
    void SetVolume(float v);
    float GetVolume();

    // Separate category volumes
    void SetBGMVolume(float v);
    float GetBGMVolume();

    void SetSFXVolume(float v);
    float GetSFXVolume();
}

#endif // AUDIO_MANAGER_HPP