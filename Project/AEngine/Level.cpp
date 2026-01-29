#include "pch.hpp"
#include "Level.hpp"
#include "AssetsManager.hpp"
#include "MapManager.hpp"

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

	cannon = CannonManager::Cannon(playerCannonStartPos, AssetsManager::GetTexture("Assets/square"), 10);
}