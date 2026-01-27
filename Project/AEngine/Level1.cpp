#include "Level1.hpp"

#include "MapManager.hpp"
#include "CannonManager.hpp"
#include "Level.hpp"

namespace Level1 {
	Level levelData;
	std::vector<std::vector<MapManager::Tile>> level1Map;

	void Load() {
		levelData.Load();
		MapManager::ReadMapData("Assets/Map/level1.txt", levelData.levelMap);
	}

	void Initialize() {
		bool foundCannonPos = false;
		// Set cannon position
		for (const std::vector<MapManager::Tile> &row : levelData.levelMap) {
			for (const MapManager::Tile &tile : row) {
				if (tile.type == MapManager::TILE_PLAYER_CANNON) {
					levelData.cannon.pos = tile.pos;
					foundCannonPos = true;
					break;
				}
			}

			if (foundCannonPos) break;
		}
	}

	void Update() {
		MapManager::RenderMap(levelData.levelMap);
		levelData.cannon.Update();
	}

	void Draw() {

	}

	void Free() {

	}

	void Unload() {

	}
}