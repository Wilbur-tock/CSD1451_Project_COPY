#include "Level1.hpp"

#include "MapManager.hpp"
#include "CannonManager.hpp"
#include "Level.hpp"
#include "CameraManager.hpp"
#include "TextObject.hpp"
#include "Utils.hpp"
#include "Tile.hpp"
#include "GraphicsManager.hpp"

namespace Level1 {
	Level levelData;

	void Load() {
		levelData.Load("Assets/Map/level1.txt");
	}

	void Init() {
		AEGfxSetBackgroundColor(0.92f, 0.92f, 0.95f);
		CameraManager::camera.SetPosition(levelData.cannon.pos);
		CameraManager::camera.SetState(CameraManager::CameraController::INPUT);
		levelData.Init();
	}

	void Update() {
		MapManager::RenderMap(levelData.levelMap);
		levelData.cannon.Update();

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

			levelData.debugText.textContent = "Grid:\nIndex X: " + std::to_string(gridX) + " Y: " + std::to_string(gridY)
				+ "\nTile: " + TileTypeToString(tile.Type()) + " " + Utils::FormatFloat(tile.attributes.GetHealth(), 2) +
				+"\nPos X: " + Utils::FormatFloat(tile.pos.x, 2) + " Y: " + Utils::FormatFloat(tile.pos.y, 2)
				+ "\nCamera " + std::to_string(CameraManager::camera.GetState()) +
				+ "\nPos X : " + Utils::FormatFloat(x, 2) + " Y : " + Utils::FormatFloat(y, 2)
				+ "\nMouse\nScreen Pos X: " + Utils::FormatFloat(mouseX, 2) + " Y: " + Utils::FormatFloat(mouseY, 2)
				+ "\nWorld Pos X: " + Utils::FormatFloat(mX, 2) + " Y: " + Utils::FormatFloat(mY, 2);
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