#ifndef ASSETSMANAGER_HPP
#define ASSETSMANAGER_HPP

#include "pch.hpp"
#include <map>

namespace AssetsManager {
	// Storage for textures
	extern std::map<std::string, AEGfxTexture*> textureCache;

	extern AEGfxTexture* squareTexture, *circleTexture;
	extern s8 textFont;

	void Load();

	// Find the texture inside the storage or else load the texture
	AEGfxTexture* GetTexture(const char* filePath);

	// Clean up all memory at the end of the game
	void Unload();
}

#endif // !ASSETSMANAHER_HPP