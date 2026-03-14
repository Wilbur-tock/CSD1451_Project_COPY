#include "pch.hpp"

#include "AssetsManager.hpp"

namespace AssetsManager {
    std::map<std::string, AEGfxTexture*> textureCache;

    // Common textures
    AEGfxTexture* squareTexture, *circleTexture;
    s8 textFont, titleFont;

    std::vector<AEGfxTexture*> playerCharacters;
    std::vector<AEGfxTexture*> enemyCharacters;
    std::vector<AEGfxTexture*> bossCharacters;

    // Type: 0 - player, 1 - enemy, 2 - boss
    std::vector<AEGfxTexture*> const& GetCharacterTexture(int type) {
        if (type == 0) return playerCharacters;
        if (type == 1) return enemyCharacters;
        else return bossCharacters;
    }

    static void LoadFromPath(std::vector<char const*> const& paths, std::vector<AEGfxTexture*>& textureList) {
        for (char const* path : paths) {
            textureList.push_back(GetTexture(path));
        }
    }

    void Load() {
        squareTexture = GetTexture("Assets/square.png");
        textFont = AEGfxCreateFont("Assets/Font/liberation-mono.ttf", Config::FONT_SIZE);
        titleFont = AEGfxCreateFont("Assets/Font/liberation-mono.ttf", Config::FONT_SIZE * 2);
        circleTexture = AssetsManager::GetTexture("Assets/circle.png");

        // Load Characters texture
        std::vector<char const*> playerPaths = {
            "Assets/Characters/playerIdle.png",
            "Assets/Characters/playerHalfHp.png",
            "Assets/Characters/playerSquat.png",
        };

        std::vector<char const*> enemyPaths = {
            "Assets/Characters/enemy100.png",
            "Assets/Characters/enemy75.png",
            "Assets/Characters/enemy50.png",
            "Assets/Characters/enemy25.png",
        };

        std::vector<char const*> bossPaths = {
            "Assets/Characters/enemyBoss.png",
        };

        LoadFromPath(playerPaths, playerCharacters);
        LoadFromPath(enemyPaths, enemyCharacters);
        LoadFromPath(bossPaths, bossCharacters);
    }

    AEGfxTexture* GetTexture(char const* filePath) {
        // Check if we already loaded this file
        auto it = textureCache.find(filePath);
        if (it != textureCache.end()) {
            return it->second; // Return the existing pointer
        }

        // If not found, load it now
        AEGfxTexture* newTex = AEGfxTextureLoad(filePath);
        if (newTex) {
            textureCache[filePath] = newTex;
        }
        return newTex;
    }

    void Unload() {
        //Unload all textures inside texture cache
        for (auto const &pair : textureCache) {
            AEGfxTextureUnload(pair.second);
        }
        textureCache.clear();
        squareTexture = nullptr;
        circleTexture = nullptr;

        // clear font
        AEGfxDestroyFont(textFont);
        AEGfxDestroyFont(titleFont);
    }
}