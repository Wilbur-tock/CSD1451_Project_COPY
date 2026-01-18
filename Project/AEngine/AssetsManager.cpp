#include "pch.hpp"

#include "AssetsManager.hpp"

namespace AssetsManager {
    std::map<std::string, AEGfxTexture*> textureCache;

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

    void UnloadAll() {
        //Unload all textures inside texture cache
        for (auto const &pair : textureCache) {
            AEGfxTextureUnload(pair.second);
        }
        textureCache.clear();
    }
}