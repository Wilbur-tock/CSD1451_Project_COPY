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
		std::cout << "Load level 1" << std::endl;
	}

	void Initalize() {
		AEGfxSetBackgroundColor(0.92f, 0.92f, 0.95f);
		levelData.Initalize();
		LevelManager::selectedLevel = &levelData;
		std::cout << "Initialize level 1" << std::endl;
	}

	void Update() {
		levelData.Update();

		AbilitiesManager::RenderUI();
		if (Utils::isDebuggerOn) {
			f32 x, y;
			AEGfxGetCamPosition(&x, &y);

			s32 mouseX, mouseY;
			AEInputGetCursorPosition(&mouseX, &mouseY);
			float mX = 0, mY = 0;
			Utils::GetWorldMousePos(mX, mY);

			int gridX = 0, gridY = 0;
			Utils::GetMouseGridIndex(gridX, gridY);

			Tile& tile = (*MapManager::currentMap)[gridY][gridX];
			
			std::string debug = "Grid:\nIndex X: " + std::to_string(gridX) + " Y: " + std::to_string(gridY)
				+ "\n\nTile: " + TileTypeToString(tile.Type())
				+ "\nPos X: " + Utils::FormatFloat(tile.pos.x, 2) + " Y: " + Utils::FormatFloat(tile.pos.y, 2)
				+ "\nHP: " + Utils::FormatFloat(tile.attribute.Health(), 2) 
				+ "\nDamage Multiplier: " + std::to_string(tile.attribute.DamageMultiplier())
				+ "\n\nCamera " + std::to_string(CameraManager::camera.GetState())
				+ "\nPos X : " + Utils::FormatFloat(x, 2) + " Y : " + Utils::FormatFloat(y, 2)
				+ "\nMouse\nScreen Pos X: " + Utils::FormatFloat(mouseX, 2) + " Y: " + Utils::FormatFloat(mouseY, 2)
				+ "\nWorld Pos X: " + Utils::FormatFloat(mX, 2) + " Y: " + Utils::FormatFloat(mY, 2);

			debug += levelData.Debugger();
			levelData.debugText.textContent = debug;

			GraphicsManager::DrawTextUI(levelData.debugText);
		}
	}

	void Draw() {

	}

	void Free() {

	}

	void Unload() {

	}
}