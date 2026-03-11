#include "pch.hpp"

#include "AssetsManager.hpp"

namespace AssetsManager {
    std::map<std::string, AEGfxTexture*> textureCache;

    // Common textures
    AEGfxTexture* squareTexture, *circleTexture;
    s8 textFont, titleFont;

    void Load() {
        squareTexture = GetTexture("Assets/square.png");
        textFont = AEGfxCreateFont("Assets/liberation-mono.ttf", Config::FONT_SIZE);
        titleFont = AEGfxCreateFont("Assets/liberation-mono.ttf", Config::FONT_SIZE * 2);
        circleTexture = AssetsManager::GetTexture("Assets/circle.png");
    }

    AEGfxTexture* GetTexture(const char* filePath) {
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