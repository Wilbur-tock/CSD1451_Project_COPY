/*
Authors: Bryan
Description: Controls and play the audio

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "AudioManager.hpp"
#include <vector>
#include <string>
#include <fstream>

namespace AudioManager {

    static Track gCurrentTrack = Track::None;
    static bool gIsInitialized = false;

    // Volume layers
    static float gMasterVolume = 1.0f;
    static float gBgmVolume = 1.0f;
    static float gSfxVolume = 1.0f;

    // Audio groups
    static AEAudioGroup gBgmGroup{};
    static AEAudioGroup gSfxGroup{};

    // ---------------- BGM ----------------
    static AEAudio gBgm{};

    // ---------------- SFX ----------------
    static AEAudio gSfxUIButton{};
    static AEAudio gSfxCannonFire{};
    static AEAudio gSfxImpact{};
    static AEAudio gSfxTurnSwitch{};

    struct AbilitySfxEntry {
        const char* abilityName;
        const char* path;
        AEAudio audio{};
    };

    static constexpr const char* AUDIO_SETTINGS_PATH = "Assets/Data/audioSettings.txt";
    static bool gSettingsDirty = false;

    static void ApplyDefaultVolumes() {
        gMasterVolume = 0.35f;
        gBgmVolume = 1.0f;
        gSfxVolume = 1.0f;
    }

    static void ClampStoredVolumes() {
        gMasterVolume = std::clamp(gMasterVolume, 0.0f, 1.0f);
        gBgmVolume = std::clamp(gBgmVolume, 0.0f, 1.0f);
        gSfxVolume = std::clamp(gSfxVolume, 0.0f, 1.0f);
    }

    // ---------------- Paths ----------------
    static constexpr const char* BGM = "Assets/Audio/bgm.mp3";

    static constexpr const char* SFX_UI_CLICK = "Assets/Audio/sfx_ui_click.mp3";
    static constexpr const char* SFX_CANNON_FIRE = "Assets/Audio/sfx_default.mp3";
    static constexpr const char* SFX_IMPACT = "Assets/Audio/sfx_impact.mp3";
    static constexpr const char* SFX_TURN_SWITCH = "Assets/Audio/sfx_turn.mp3";

    static std::vector<AbilitySfxEntry> gAbilitySfx = {
        { "Default",       "Assets/Audio/sfx_default.mp3", {} },
        { "Attack Boost",  "Assets/Audio/sfx_ability_attack_boost.mp3", {} },
        { "Auto Shot",     "Assets/Audio/sfx_ability_auto_shot.mp3", {} },
        { "Bomb",          "Assets/Audio/sfx_ability_bomb.mp3", {} },
        { "Cleanse",       "Assets/Audio/sfx_ability_cleanse.mp3", {} },
        { "Defense Boost", "Assets/Audio/sfx_ability_defense_boost.mp3", {} },
        { "Drill",         "Assets/Audio/sfx_ability_drill.mp3", {} },
        { "Gravity",       "Assets/Audio/sfx_ability_gravity.mp3", {} },

        { "Minor Heal",    "Assets/Audio/sfx_ability_heal.mp3", {} },
        { "Major Heal",    "Assets/Audio/sfx_ability_heal.mp3", {} },
        { "Max Heal",      "Assets/Audio/sfx_ability_heal.mp3", {} },

        { "Invincibility", "Assets/Audio/sfx_ability_invincibility.mp3", {} },
        { "God Mode",  "Assets/Audio/sfx_ability_god_mode.mp3", {} },
        { "Nuke",          "Assets/Audio/sfx_ability_nuke.mp3", {} },
        { "Penta Burst",   "Assets/Audio/sfx_ability_penta_burst.mp3", {} },
        { "Piercing",      "Assets/Audio/sfx_ability_piercing.mp3", {} },
        { "Poison",        "Assets/Audio/sfx_ability_poison.mp3", {} },
        { "Rebuild",       "Assets/Audio/sfx_ability_rebuild.mp3", {} },
        { "Shield Buster", "Assets/Audio/sfx_ability_shield_buster.mp3", {} },
        { "Skip Turn",     "Assets/Audio/sfx_ability_skip_turn.mp3", {} },
        { "Teleport",      "Assets/Audio/sfx_ability_teleport.mp3", {} },
        { "Tower Shield",  "Assets/Audio/sfx_ability_tower_shield.mp3", {} },

        { "Small Wall",    "Assets/Audio/sfx_ability_wall.mp3", {} },
        { "Wall",          "Assets/Audio/sfx_ability_wall.mp3", {} },
        { "Bastion",       "Assets/Audio/sfx_ability_wall.mp3", {} },
        { "Wall Buster",   "Assets/Audio/sfx_ability_wall.mp3", {} },

        { "Air Drop",      "Assets/Audio/sfx_ability_air_drop.mp3", {} },

        { "Cluster Shot",  "Assets/Audio/sfx_ability_penta_burst.mp3", {} },
        { "Extra Shot",    "Assets/Audio/sfx_ability_penta_burst.mp3", {} },
        { "Decay Shot",    "Assets/Audio/sfx_ability_gravity.mp3", {} }
    };

    static bool IsValidAudio(AEAudio audio) {
        return AEAudioIsValidAudio(audio) != 0;
    }

    static bool IsValidGroup(AEAudioGroup group) {
        return AEAudioIsValidGroup(group) != 0;
    }

    static void WarnIfInvalidAudio(const char* label, AEAudio audio) {
        if (!IsValidAudio(audio)) {
            std::cerr << "[AudioManager] Failed to load audio: " << label << "\n";
        }
    }

    static void ApplyVolumes() {
        if (IsValidGroup(gBgmGroup)) {
            AEAudioSetGroupVolume(gBgmGroup, gMasterVolume * gBgmVolume);
        }

        if (IsValidGroup(gSfxGroup)) {
            AEAudioSetGroupVolume(gSfxGroup, gMasterVolume * gSfxVolume);
        }
    }

    static AEAudio GetTrackAudio(Track t) {
        switch (t) {
        case Track::MainMenu:
        case Track::MapEditor:
        case Track::Level:
        case Track::Credits:
            return gBgm;
        default:
            return AEAudio{};
        }
    }
    static AEAudio GetSfxAudio(SFX sfx) {
        switch (sfx) {
        case SFX::UIButton:   return gSfxUIButton;
        case SFX::CannonFire: return gSfxCannonFire;
        case SFX::Impact:     return gSfxImpact;
        case SFX::TurnSwitch: return gSfxTurnSwitch;
        default:              return AEAudio{};
        }
    }

    static AEAudio GetAbilitySfxAudio(const std::string& abilityName) {
        for (const AbilitySfxEntry& entry : gAbilitySfx) {
            if (abilityName == entry.abilityName) {
                return entry.audio;
            }
        }
        return AEAudio{};
    }

    static bool IsInstantAbilitySfx(const std::string& abilityName) {
        return abilityName == "Minor Heal" ||
            abilityName == "Major Heal" ||
            abilityName == "Max Heal" ||
            abilityName == "Cleanse" ||
            abilityName == "Rebuild" ||
            abilityName == "Skip Turn" ||
            abilityName == "Tower Shield" ||
            abilityName == "Invisibility" ||
            abilityName == "God Mode" ||
            abilityName == "Attack Boost" ||
            abilityName == "Defense Boost" ||
            abilityName == "Extra Shot";
    }

    static bool IsLaunchAbilitySfx(const std::string& abilityName) {
        return abilityName == "Auto Shot" ||
            abilityName == "Bomb" ||
            abilityName == "Drill" ||
            abilityName == "Gravity" ||
            abilityName == "Nuke" ||
            abilityName == "Penta Burst" ||
            abilityName == "Piercing" ||
            abilityName == "Poison" ||
            abilityName == "Shield Buster" ||
            abilityName == "Teleport" ||
            abilityName == "Wall Buster" ||
            abilityName == "Air Drop" ||
            abilityName == "Cluster Shot" ||
            abilityName == "Decay Shot";
    }

    static void PlayAbilitySfxInternal(const std::string& abilityName) {
        if (!gIsInitialized) {
            Load();
        }

        if (!gIsInitialized) {
            return;
        }

        if (!IsValidGroup(gSfxGroup)) {
            std::cerr << "[AudioManager] Cannot play ability SFX: invalid SFX group.\n";
            return;
        }

        AEAudio audio = GetAbilitySfxAudio(abilityName);
        if (!IsValidAudio(audio)) {
            return;
        }

        AEAudioPlay(audio, gSfxGroup, 1.0f, 1.0f, 0);
        ApplyVolumes();
    }

    void PlayInstantAbilitySFX(const std::string& abilityName) {
        if (!IsInstantAbilitySfx(abilityName)) {
            return;
        }

        PlayAbilitySfxInternal(abilityName);
    }

    void PlayLaunchAbilitySFX(const std::string& abilityName) {
        if (!IsLaunchAbilitySfx(abilityName)) {
            return;
        }

        PlayAbilitySfxInternal(abilityName);
    }

    void SaveSettings() {
        ClampStoredVolumes();

        std::ofstream ofs{ AUDIO_SETTINGS_PATH, std::ios::trunc };
        if (!ofs.is_open()) {
            std::cerr << "[AudioManager] Failed to save audio settings: "
                << AUDIO_SETTINGS_PATH << "\n";
            return;
        }

        ofs << "MasterVolume: " << gMasterVolume << "\n";
        ofs << "BGMVolume: " << gBgmVolume << "\n";
        ofs << "SFXVolume: " << gSfxVolume << "\n";

        gSettingsDirty = false;
    }

    void LoadSettings() {
        ApplyDefaultVolumes();

        std::ifstream ifs{ AUDIO_SETTINGS_PATH };
        if (!ifs.is_open()) {
            SaveSettings();
            return;
        }

        std::string label;
        float master = gMasterVolume;
        float bgm = gBgmVolume;
        float sfx = gSfxVolume;

        while (ifs >> label) {
            if (label == "MasterVolume:") {
                ifs >> master;
            }
            else if (label == "BGMVolume:") {
                ifs >> bgm;
            }
            else if (label == "SFXVolume:") {
                ifs >> sfx;
            }
        }

        gMasterVolume = master;
        gBgmVolume = bgm;
        gSfxVolume = sfx;

        ClampStoredVolumes();
        gSettingsDirty = false;
    }

    void Load() {
        if (gIsInitialized) {
            return;
        }

        gBgmGroup = AEAudioCreateGroup();
        gSfxGroup = AEAudioCreateGroup();

        if (!IsValidGroup(gBgmGroup)) {
            std::cerr << "[AudioManager] Failed to create BGM group.\n";
        }

        if (!IsValidGroup(gSfxGroup)) {
            std::cerr << "[AudioManager] Failed to create SFX group.\n";
        }

        gBgm = AEAudioLoadMusic(BGM);
        WarnIfInvalidAudio(BGM, gBgm);

        gSfxUIButton = AEAudioLoadSound(SFX_UI_CLICK);
        gSfxCannonFire = AEAudioLoadSound(SFX_CANNON_FIRE);
        gSfxImpact = AEAudioLoadSound(SFX_IMPACT);
        gSfxTurnSwitch = AEAudioLoadSound(SFX_TURN_SWITCH);

        WarnIfInvalidAudio(SFX_UI_CLICK, gSfxUIButton);
        WarnIfInvalidAudio(SFX_CANNON_FIRE, gSfxCannonFire);
        WarnIfInvalidAudio(SFX_IMPACT, gSfxImpact);
        WarnIfInvalidAudio(SFX_TURN_SWITCH, gSfxTurnSwitch);

        for (AbilitySfxEntry& entry : gAbilitySfx) {
            entry.audio = AEAudioLoadSound(entry.path);
            WarnIfInvalidAudio(entry.path, entry.audio);
        }

        gCurrentTrack = Track::None;

        LoadSettings();
        ClampStoredVolumes();
        ApplyVolumes();

        gIsInitialized = true;
    }

    void Update() {
        // AEAudioUpdate() is already handled by AlphaEngine
    }

    void Shutdown() {
        if (!gIsInitialized) {
            return;
        }

        if (gSettingsDirty) {
            SaveSettings();
        }

        if (IsValidGroup(gBgmGroup)) {
            AEAudioStopGroup(gBgmGroup);
        }

        if (IsValidGroup(gSfxGroup)) {
            AEAudioStopGroup(gSfxGroup);
        }

        if (IsValidAudio(gBgm)) AEAudioUnloadAudio(gBgm);

        if (IsValidAudio(gSfxUIButton))   AEAudioUnloadAudio(gSfxUIButton);
        if (IsValidAudio(gSfxCannonFire)) AEAudioUnloadAudio(gSfxCannonFire);
        if (IsValidAudio(gSfxImpact))     AEAudioUnloadAudio(gSfxImpact);
        if (IsValidAudio(gSfxTurnSwitch)) AEAudioUnloadAudio(gSfxTurnSwitch);

        for (AbilitySfxEntry& entry : gAbilitySfx) {
            if (IsValidAudio(entry.audio)) {
                AEAudioUnloadAudio(entry.audio);
            }
            entry.audio = AEAudio{};
        }

        if (IsValidGroup(gBgmGroup)) AEAudioUnloadAudioGroup(gBgmGroup);
        if (IsValidGroup(gSfxGroup)) AEAudioUnloadAudioGroup(gSfxGroup);

        gBgmGroup = AEAudioGroup{};
        gSfxGroup = AEAudioGroup{};

        gBgm = AEAudio{};

        gSfxUIButton = AEAudio{};
        gSfxCannonFire = AEAudio{};
        gSfxImpact = AEAudio{};
        gSfxTurnSwitch = AEAudio{};

        gCurrentTrack = Track::None;
        gIsInitialized = false;
    }

    void PlayBGM(Track t) {
        if (!gIsInitialized) {
            Load();
        }

        if (!gIsInitialized) {
            return;
        }

        if (t == Track::None) {
            StopBGM();
            return;
        }

        if (t == gCurrentTrack) {
            return;
        }

        if (!IsValidGroup(gBgmGroup)) {
            std::cerr << "[AudioManager] Cannot play BGM: invalid BGM group.\n";
            return;
        }

        AEAudio audio = GetTrackAudio(t);
        if (!IsValidAudio(audio)) {
            std::cerr << "[AudioManager] Cannot play BGM: track not loaded.\n";
            return;
        }

        AEAudioStopGroup(gBgmGroup);
        AEAudioPlay(audio, gBgmGroup, 1.0f, 1.0f, -1);

        gCurrentTrack = t;
        ApplyVolumes();
    }

    void StopBGM() {
        if (IsValidGroup(gBgmGroup)) {
            AEAudioStopGroup(gBgmGroup);
        }

        gCurrentTrack = Track::None;
    }

    void PlaySFX(SFX sfx) {
        if (!gIsInitialized) {
            Load();
        }

        if (!gIsInitialized) {
            return;
        }

        if (!IsValidGroup(gSfxGroup)) {
            std::cerr << "[AudioManager] Cannot play SFX: invalid SFX group.\n";
            return;
        }

        AEAudio audio = GetSfxAudio(sfx);
        if (!IsValidAudio(audio)) {
            std::cerr << "[AudioManager] Cannot play SFX: sound not loaded.\n";
            return;
        }

        AEAudioPlay(audio, gSfxGroup, 1.0f, 1.0f, 0);
        ApplyVolumes();
    }

    void SetVolume(float v) {
        gMasterVolume = std::clamp(v, 0.0f, 1.0f);
        gSettingsDirty = true;
        ApplyVolumes();
    }

    float GetVolume() {
        return gMasterVolume;
    }

    void SetBGMVolume(float v) {
        gBgmVolume = std::clamp(v, 0.0f, 1.0f);
        gSettingsDirty = true;
        ApplyVolumes();
    }

    float GetBGMVolume() {
        return gBgmVolume;
    }

    void SetSFXVolume(float v) {
        gSfxVolume = std::clamp(v, 0.0f, 1.0f);
        gSettingsDirty = true;
        ApplyVolumes();
    }

    float GetSFXVolume() {
        return gSfxVolume;
    }
}