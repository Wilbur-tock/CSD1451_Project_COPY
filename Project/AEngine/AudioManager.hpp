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

    void PlayBGM(Track t);
    void StopBGM();
    void PlaySFX(SFX sfx);
    void PlayAbilitySFX(const std::string& abilityName);

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