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

    // Init once at game start
    void Init();

    // Stop any playing music and cleanup
    void Shutdown();

    // Play a predefined track (looped). Does nothing if already playing same track.
    void PlayBGM(Track t);

    // Stop background music
    void StopBGM();

    // Optional: set output volume [0.0 .. 1.0]
    void SetVolume(float v);

    Track GetCurrentTrack();
}

#endif // AUDIO_MANAGER_HPP