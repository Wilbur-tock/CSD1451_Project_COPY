#include "pch.hpp"
#include "Level.hpp"
#include "AssetsManager.hpp"
#include "MapManager.hpp"
#include "Color.hpp"

void Level::Load(std::string filePath) {
	levelMap.resize(Config::GRID_HEIGHT, std::vector<MapManager::Tile>(Config::GRID_WIDTH));

	// Read map data
	MapManager::ReadMapData(filePath, levelMap);

	bool foundCannonPos = false;
	// Find cannon starting position
	for (const std::vector<MapManager::Tile>& row : levelMap) {
		for (const MapManager::Tile& tile : row) {
			if (tile.type == MapManager::TILE_PLAYER_CANNON) {
				playerCannonStartPos = tile.pos;
				foundCannonPos = true;
				break;
			}
		}

		if (foundCannonPos) break;
	}

	AEGfxTexture* barrelTex = AssetsManager::GetTexture("Assets/cannonBarrel.png");
	if (!barrelTex)
		barrelTex = AssetsManager::GetTexture("Assets/square.png"); // fallback

	// Pick barrel half-size (these are SCALE values, not full pixels)
	// Your old barrel used {30,10} (meaning it appears ~60x20 pixels)
	AEVec2 barrelHalfSize{ 60, 20 };

	cannon = CannonManager::Cannon(playerCannonStartPos, barrelTex, 10, barrelHalfSize);
	cannon.textureColor = Color{ 1, 1, 1, 1 };   // black barrel
}