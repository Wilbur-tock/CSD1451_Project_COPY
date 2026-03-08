#ifndef AUDIO_MANAGER_HPP
#define AUDIO_MANAGER_HPP

#include "pch.hpp" // for std::string

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

    // Init once at game start
    void Init();

    void Update();

    // Stop any playing music and cleanup
    void Shutdown();

    // Play a predefined track (looped). Does nothing if already playing same track.
    void PlayBGM(Track t);

    // Stop background music
    void StopBGM();

    void PlaySFX(SFX sfx);

    void SetVolume(float v);

    float GetVolume();
}

#endif // AUDIO_MANAGER_HPP