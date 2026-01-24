#include "Level1.hpp"

#include "MapManager.hpp"
#include "CannonManager.hpp"
#include "Level.hpp"

namespace Level1 {
	Level levelData;
	std::vector<std::vector<MapManager::Tile>> level1Map;

	void Load() {
		levelData.Load();
		MapManager::LoadMapData("Assets/Map/level1.txt", levelData.levelMap);
	}

	void Initialize() {
		levelData.cannon.pos.x = -700;
		levelData.cannon.pos.y = -250;
	}

	void Update() {
		MapManager::RenderMap(MapManager::mapLayout);
		levelData.cannon.Update();
	}

	void Draw() {

	}

	void Free() {

	}

	void Unload() {

	}
}