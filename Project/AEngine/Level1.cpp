#include "Level1.hpp"

#include "MapManager.hpp"
#include "CannonManager.hpp"
#include "Level.hpp"
#include "CameraManager.hpp"
#include "TextObject.hpp"
#include "Utils.hpp"
#include "Tile.hpp"
#include "GraphicsManager.hpp"
#include "LevelManager.hpp"
#include "AbilitiesManager.hpp"

namespace Level1 {
	Level levelData;

	void Load() {
		levelData.Load("Assets/Map/level1.txt");
	}

	void Initalize() {
		levelData.Initalize();
	}

	void Update() {
		levelData.Update();

		Draw();
	}

	void Draw() {
		AbilitiesManager::RenderUI();
	}

	void Free() {
		levelData.Free();
	}

	void Unload() {

	}
}