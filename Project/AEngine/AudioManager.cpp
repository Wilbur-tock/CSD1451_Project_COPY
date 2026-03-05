#include "AudioManager.hpp"

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

namespace AudioManager {

    static Track gCurrentTrack = Track::None;
    static float gVolume = 1.0f;

    // Put your music files here (WAV recommended for PlaySound)
    static const char* BGM_MAINMENU = "Assets/bgm_mainmenu.wav";
    static const char* BGM_LEVEL = "Assets/bgm_level.wav";
    static const char* BGM_CREDITS = "Assets/bgm_credits.wav";

#ifdef _WIN32
    static void ApplyVolume(float v) {
        if (v < 0.0f) v = 0.0f;
        if (v > 1.0f) v = 1.0f;

        DWORD vol = static_cast<DWORD>(v * 0xFFFF);
        DWORD both = (vol & 0xFFFF) | (vol << 16); // left + right
        waveOutSetVolume(nullptr, both);
    }

    static const char* TrackToPath(Track t) {
        switch (t) {
        case Track::MainMenu: return BGM_MAINMENU;
        case Track::Level:    return BGM_LEVEL;
        case Track::Credits:  return BGM_CREDITS;
        default:              return nullptr;
        }
    }
#endif

    void Init() {
#ifdef _WIN32
        ApplyVolume(gVolume);
#endif
    }

    void Shutdown() {
        StopBGM();
    }

    void SetVolume(float v) {
        gVolume = v;
#ifdef _WIN32
        ApplyVolume(gVolume);
#endif
    }

    void PlayBGM(Track t) {
        if (t == gCurrentTrack) return; // prevent restarting every time
        gCurrentTrack = t;

#ifdef _WIN32
        const char* path = TrackToPath(t);
        if (!path) {
            PlaySoundA(nullptr, nullptr, 0); // stop
            return;
        }

        // Loop + async so game doesn't stall
        BOOL ok = PlaySoundA(path, nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
        if (!ok) {
            std::cerr << "[AudioManager] Failed to play BGM: " << path << "\n";
        }
#else
        (void)t;
#endif
    }

    void StopBGM() {
        gCurrentTrack = Track::None;
#ifdef _WIN32
        PlaySoundA(nullptr, nullptr, 0);
#endif
    }
}