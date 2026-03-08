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

    static unsigned int gNextSfxId = 0;
    static const std::string gBgmAlias = "bgm";
    static std::vector<std::string> gActiveSfxAliases;

    // ---------------- BGM ----------------
    static const char* BGM_MAINMENU = "Assets/bgm_mainmenu.wav";
    static const char* BGM_MAPEDITOR = "Assets/bgm_mapeditor.wav";
    static const char* BGM_LEVEL = "Assets/bgm_level.wav";
    static const char* BGM_CREDITS = "Assets/bgm_credits.wav";

    // ---------------- SFX ----------------
    static const char* SFX_UI_CLICK = "Assets/sfx_ui_click.wav";
    static const char* SFX_CANNON_FIRE = "Assets/sfx_cannon_fire.wav";
    static const char* SFX_IMPACT = "Assets/sfx_impact.wav";
    static const char* SFX_TURN_SWITCH = "Assets/sfx_turn.wav";

#ifdef _WIN32
    static bool FileExists(const char* path) {
        DWORD attrs = GetFileAttributesA(path);
        return (attrs != INVALID_FILE_ATTRIBUTES &&
            !(attrs & FILE_ATTRIBUTE_DIRECTORY));
    }

    static void ApplyVolume(float v) {
        if (v < 0.0f) v = 0.0f;
        if (v > 1.0f) v = 1.0f;

        DWORD vol = static_cast<DWORD>(v * 0xFFFF);
        DWORD both = (vol & 0xFFFF) | (vol << 16);
        waveOutSetVolume(nullptr, both);
    }

    static bool MciCommand(const std::string& cmd, std::string* out = nullptr) {
        char buffer[256] = {};
        MCIERROR err = mciSendStringA(
            cmd.c_str(),
            out ? buffer : nullptr,
            out ? static_cast<UINT>(sizeof(buffer)) : 0,
            nullptr
        );

        if (err != 0) {
            char errText[256] = {};
            mciGetErrorStringA(err, errText, sizeof(errText));
            std::cerr << "[AudioManager] MCI failed: " << cmd << "\n"
                << "[AudioManager] Reason: " << errText << "\n";
            return false;
        }

        if (out) {
            *out = buffer;
        }
        return true;
    }

    static bool OpenWave(const char* path, const std::string& alias) {
        return MciCommand("open \"" + std::string(path) + "\" type waveaudio alias " + alias);
    }

    static void CloseAlias(const std::string& alias) {
        MciCommand("close " + alias);
    }

    static std::string GetMode(const std::string& alias) {
        std::string mode;
        if (!MciCommand("status " + alias + " mode", &mode)) {
            return "";
        }
        return mode;
    }

    static const char* TrackToPath(Track t) {
        switch (t) {
        case Track::MainMenu:  return BGM_MAINMENU;
        case Track::MapEditor: return BGM_MAPEDITOR;
        case Track::Level:     return BGM_LEVEL;
        case Track::Credits:   return BGM_CREDITS;
        default:               return nullptr;
        }
    }

    static const char* SfxToPath(SFX sfx) {
        switch (sfx) {
        case SFX::UIButton:   return SFX_UI_CLICK;
        case SFX::CannonFire: return SFX_CANNON_FIRE;
        case SFX::Impact:     return SFX_IMPACT;
        case SFX::TurnSwitch: return SFX_TURN_SWITCH;
        default:              return nullptr;
        }
    }
#endif

    void Init() {
#ifdef _WIN32
        ApplyVolume(gVolume);
#endif
    }

    void Update() {
#ifdef _WIN32
        // Clean up finished SFX aliases
        for (auto it = gActiveSfxAliases.begin(); it != gActiveSfxAliases.end(); ) {
            std::string mode = GetMode(*it);

            if (mode.empty() || mode == "stopped" || mode == "not ready") {
                CloseAlias(*it);
                it = gActiveSfxAliases.erase(it);
            }
            else {
                ++it;
            }
        }

        // Manual BGM looping
        if (gCurrentTrack != Track::None) {
            std::string bgmMode = GetMode(gBgmAlias);

            if (bgmMode == "stopped") {
                MciCommand("seek " + gBgmAlias + " to start");
                MciCommand("play " + gBgmAlias);
            }
        }
#endif
    }

    void Shutdown() {
        StopBGM();

#ifdef _WIN32
        for (const std::string& alias : gActiveSfxAliases) {
            CloseAlias(alias);
        }
#endif
        gActiveSfxAliases.clear();
    }

    void SetVolume(float v) {
        gVolume = v;
#ifdef _WIN32
        ApplyVolume(gVolume);
#endif
    }

    float GetVolume() {
        return gVolume;
    }

    void PlayBGM(Track t) {
        if (t == gCurrentTrack) return;

        StopBGM();

#ifdef _WIN32
        const char* path = TrackToPath(t);
        if (!path) {
            gCurrentTrack = Track::None;
            return;
        }

        if (!FileExists(path)) {
            std::cerr << "[AudioManager] Missing BGM file: " << path << "\n";
            gCurrentTrack = Track::None;
            return;
        }

        if (!OpenWave(path, gBgmAlias)) {
            gCurrentTrack = Track::None;
            return;
        }

        if (!MciCommand("play " + gBgmAlias)) {
            std::cerr << "[AudioManager] Failed to play BGM: " << path << "\n";
            CloseAlias(gBgmAlias);
            gCurrentTrack = Track::None;
            return;
        }

        gCurrentTrack = t;
#else
        (void)t;
#endif
    }

    void StopBGM() {
#ifdef _WIN32
        if (gCurrentTrack != Track::None) {
            MciCommand("stop " + gBgmAlias);
            CloseAlias(gBgmAlias);
        }
#endif
        gCurrentTrack = Track::None;
    }

    void PlaySFX(SFX sfx) {
#ifdef _WIN32
        const char* path = SfxToPath(sfx);
        if (!path) return;

        if (!FileExists(path)) {
            std::cerr << "[AudioManager] Missing SFX file: " << path << "\n";
            return;
        }

        std::string alias = "sfx_" + std::to_string(++gNextSfxId);

        if (!OpenWave(path, alias)) {
            return;
        }

        if (!MciCommand("play " + alias)) {
            CloseAlias(alias);
            return;
        }

        gActiveSfxAliases.push_back(alias);
#else
        (void)sfx;
#endif
    }
}