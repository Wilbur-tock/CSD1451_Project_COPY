/*
Authors: Dayrius, Bryan
Description: Managed the loading and unloading of all textures asset in the game

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef ASSETSMANAGER_HPP
#define ASSETSMANAGER_HPP

#include "pch.hpp"
#include <map>

namespace AssetsManager {
	// Storage for textures
	extern std::map<std::string, AEGfxTexture*> textureCache;

	extern AEGfxTexture* squareTexture, *circleTexture;
	extern s8 textFont;

	// Type: 0 - player, 1 - enemy, 2 - boss
	std::vector<AEGfxTexture*> const& GetCharacterTexture(int type);

	void Load();

	// Find the texture inside the storage or else load the texture
	AEGfxTexture* GetTexture(const char* filePath);

	// Clean up all memory at the end of the game
	void Unload();
}

#endif // !ASSETSMANAHER_HPP