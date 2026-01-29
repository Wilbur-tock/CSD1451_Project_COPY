#include "Level1.hpp"

#include "MapManager.hpp"
#include "CannonManager.hpp"
#include "Level.hpp"
#include "CameraManager.hpp"

namespace Level1 {
	Level levelData;
	std::vector<std::vector<MapManager::Tile>> level1Map;

	void Load() {
		levelData.Load("Assets/Map/level1.txt");
	}

	void Initialize() {
		CameraManager::camera.SetPosition(levelData.cannon.pos);
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