/*
Authors: Bryan
Description: Controls and play the audio

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "AudioManager.hpp"
#include <vector>
#include <string>

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
    static AEAudio gBgmMainMenu{};
    static AEAudio gBgmMapEditor{};
    static AEAudio gBgmLevel{};
    static AEAudio gBgmCredits{};

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

    // ---------------- Paths ----------------
    static constexpr const char* BGM_MAINMENU       = "Assets/Audio/bgm_mainmenu.wav";
    static constexpr const char* BGM_MAPEDITOR      = "Assets/Audio/bgm_mapeditor.wav";
    static constexpr const char* BGM_LEVEL          = "Assets/Audio/bgm_level.wav";
    static constexpr const char* BGM_CREDITS        = "Assets/Audio/bgm_credits.wav";
    static constexpr const char* SFX_UI_CLICK       = "Assets/Audio/sfx_ui_click.wav";
    static constexpr const char* SFX_CANNON_FIRE    = "Assets/Audio/sfx_default.wav";
    static constexpr const char* SFX_IMPACT         = "Assets/Audio/sfx_impact.wav";
    static constexpr const char* SFX_TURN_SWITCH    = "Assets/Audio/sfx_turn.wav";

    static std::vector<AbilitySfxEntry> gAbilitySfx = {
    { "Default",       "Assets/Audio/sfx_default.wav", {} },
    { "Attack Boost",  "Assets/Audio/sfx_ability_attack_boost.wav", {} },
    { "Auto Shot",      "Assets/Audio/sfx_ability_auto_shot.wav", {} },
    { "Bomb",          "Assets/Audio/sfx_ability_bomb.wav", {} },
    { "Cleanse",       "Assets/Audio/sfx_ability_cleanse.wav", {} },
    { "Defense Boost", "Assets/Audio/sfx_ability_defense_boost.wav", {} },
    { "Drill",         "Assets/Audio/sfx_ability_drill.wav", {} },
    { "Gravity",       "Assets/Audio/sfx_ability_gravity.wav", {} },

    { "Minor Heal",    "Assets/Audio/sfx_ability_heal.wav", {} },
    { "Major Heal",    "Assets/Audio/sfx_ability_heal.wav", {} },
    { "Max Heal",      "Assets/Audio/sfx_ability_heal.wav", {} },

    { "Invincibility", "Assets/Audio/sfx_ability_invincibility.wav", {} },
    { "Invisibility",  "Assets/Audio/sfx_ability_invisibility.wav", {} },
    { "Nuke",          "Assets/Audio/sfx_ability_nuke.wav", {} },
    { "Penta Burst",   "Assets/Audio/sfx_ability_penta_burst.wav", {} },
    { "Piercing",      "Assets/Audio/sfx_ability_piercing.wav", {} },
    { "Poison",        "Assets/Audio/sfx_ability_poison.wav", {} },
    { "Rebuild",       "Assets/Audio/sfx_ability_rebuild.wav", {} },
    { "Shield Buster", "Assets/Audio/sfx_ability_shield_buster.wav", {} },
    { "Skip Turn",     "Assets/Audio/sfx_ability_skip_turn.wav", {} },
    { "Teleport",      "Assets/Audio/sfx_ability_teleport.wav", {} },
    { "Tower Shield",  "Assets/Audio/sfx_ability_tower_shield.wav", {} },

    { "Small Wall",    "Assets/Audio/sfx_ability_wall.wav", {} },
    { "Wall",          "Assets/Audio/sfx_ability_wall.wav", {} },
    { "Bastion",       "Assets/Audio/sfx_ability_wall.wav", {} },
    { "Wall Buster",   "Assets/Audio/sfx_ability_wall.wav", {} },

    { "Air Drop",      "Assets/Audio/sfx_ability_air_drop.wav", {} },

    { "Cluster Shot",  "Assets/Audio/sfx_ability_penta_burst.wav", {} },
    { "Extra Shot",    "Assets/Audio/sfx_ability_penta_burst.wav", {} },
    { "Decay Shot",    "Assets/Audio/sfx_ability_gravity.wav", {} }
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
        case Track::MainMenu:  return gBgmMainMenu;
        case Track::MapEditor: return gBgmMapEditor;
        case Track::Level:     return gBgmLevel;
        case Track::Credits:   return gBgmCredits;
        default:               return AEAudio{};
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

        gBgmMainMenu = AEAudioLoadMusic(BGM_MAINMENU);
        gBgmMapEditor = AEAudioLoadMusic(BGM_MAPEDITOR);
        gBgmLevel = AEAudioLoadMusic(BGM_LEVEL);
        gBgmCredits = AEAudioLoadMusic(BGM_CREDITS);

        WarnIfInvalidAudio(BGM_MAINMENU, gBgmMainMenu);
        WarnIfInvalidAudio(BGM_MAPEDITOR, gBgmMapEditor);
        WarnIfInvalidAudio(BGM_LEVEL, gBgmLevel);
        WarnIfInvalidAudio(BGM_CREDITS, gBgmCredits);

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

        gMasterVolume = std::clamp(gMasterVolume, 0.0f, 1.0f);
        gBgmVolume = std::clamp(gBgmVolume, 0.0f, 1.0f);
        gSfxVolume = std::clamp(gSfxVolume, 0.0f, 1.0f);

        ApplyVolumes();

        // Starting variables for audio
        SetVolume(0.35f); // Master volume
        SetBGMVolume(1.0f);
        SetSFXVolume(1.0f);

        gIsInitialized = true;
    }

    void Update() {
        // AEAudioUpdate() is already handled by AlphaEngine
    }

    void Shutdown() {
        if (!gIsInitialized) {
            return;
        }

        if (IsValidGroup(gBgmGroup)) {
            AEAudioStopGroup(gBgmGroup);
        }

        if (IsValidGroup(gSfxGroup)) {
            AEAudioStopGroup(gSfxGroup);
        }

        if (IsValidAudio(gBgmMainMenu))  AEAudioUnloadAudio(gBgmMainMenu);
        if (IsValidAudio(gBgmMapEditor)) AEAudioUnloadAudio(gBgmMapEditor);
        if (IsValidAudio(gBgmLevel))     AEAudioUnloadAudio(gBgmLevel);
        if (IsValidAudio(gBgmCredits))   AEAudioUnloadAudio(gBgmCredits);

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

        gBgmMainMenu = AEAudio{};
        gBgmMapEditor = AEAudio{};
        gBgmLevel = AEAudio{};
        gBgmCredits = AEAudio{};

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

    void PlayAbilitySFX(const std::string& abilityName) {
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

    void SetVolume(float v) {
        gMasterVolume = std::clamp(v, 0.0f, 1.0f);
        ApplyVolumes();
    }

    float GetVolume() {
        return gMasterVolume;
    }

    void SetBGMVolume(float v) {
        gBgmVolume = std::clamp(v, 0.0f, 1.0f);
        ApplyVolumes();
    }

    float GetBGMVolume() {
        return gBgmVolume;
    }

    void SetSFXVolume(float v) {
        gSfxVolume = std::clamp(v, 0.0f, 1.0f);
        ApplyVolumes();
    }

    float GetSFXVolume() {
        return gSfxVolume;
    }
}