#include "pch.hpp"

#include "MapEditor.hpp"
#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Utils.hpp"

using namespace MapManager;

namespace MapEditor {
	TileType defaultTile = Tile_Block_1;
	TileType drawingBlockType = defaultTile;
	bool isDrawingNewMap = true;

	std::vector<std::vector<Tile>> newMap(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));

	// If the Project/Assets/Map does not contain default.txt file 
	// it will create a new one if not just return
	static void CreateDefaultMap() {
		const char* fileName = "../../Assets/Map/default.txt";
		std::ifstream ifs{ fileName };
		if (ifs) {
			std::cout << "Contain default.txt " << fileName << std::endl;
			return;
		}

		std::ofstream output{ fileName };
	
		if (!output.is_open()) {
			std::cerr << "Error: Could not open file at " << fileName << std::endl;
			return;
		}
	
		for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
			for (int x = 0; x < Config::GRID_WIDTH; ++x) {
				int val = newMap[y][x].type;
				output << val << " ";
			}
			output << std::endl;
		}
	
		output.close();
	}

	static void InitMapEditor() {
		// Reset settings
		drawingBlockType = defaultTile;
		CreateDefaultMap();
		// Reset map
		MapManager::LoadMapData("Assets/Map/default.txt", newMap);
	}

	void MapEditor() {
		if (isDrawingNewMap) {
			InitMapEditor();
			isDrawingNewMap = false;
		}

		for (int i = 0; i < MapManager::Tile_Count; ++i) {
			if (AEInputCheckTriggered(AEVK_1+i)) {
				drawingBlockType = static_cast<MapManager::TileType>(i+1);
			}
		}

		// Calculate Grid x and y index
		float worldMouseX = 0, worldMouseY = 0;
		Utils::GetWorldMousePos(worldMouseX, worldMouseY);

		int gridX = static_cast<int>((worldMouseX + Config::WORLD_MAX_X) / Config::TILE_SIZE);
		int gridY = static_cast<int>((Config::WORLD_MAX_Y - worldMouseY) / Config::TILE_SIZE);

		// This is to ensure that the grid index does not go out of range
		Utils::Limit(gridX, Config::MIN_GRID_X, Config::MAX_GRID_X);
		Utils::Limit(gridY, Config::MIN_GRID_Y, Config::MAX_GRID_Y);

		// Check for input to draw the map editor
		if (gridX >= 0 && gridX < Config::GRID_WIDTH && gridY >= 0 && gridY < Config::GRID_HEIGHT) {
			if (AEInputCheckCurr(AEVK_LBUTTON)) {
				newMap[gridY][gridX].type = drawingBlockType;

				if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
					for (int i = 0; i < Config::GRID_WIDTH; ++i) {
						newMap[gridY][i].type = drawingBlockType;
					}
				}

				if (AEInputCheckCurr(AEVK_LSHIFT) || AEInputCheckCurr(AEVK_RSHIFT)) {
					for (int i = 0; i < Config::GRID_HEIGHT; ++i) {
						newMap[i][gridX].type = drawingBlockType;
					}
				}
			}
			else if (AEInputCheckCurr(AEVK_RBUTTON)) {

				newMap[gridY][gridX].type = MapManager::Tile_Air;

				if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
					for (int i = 0; i < Config::GRID_WIDTH; ++i) {
						newMap[gridY][i].type = MapManager::Tile_Air;
					}
				}

				if (AEInputCheckCurr(AEVK_LSHIFT) || AEInputCheckCurr(AEVK_RSHIFT)) {
					for (int i = 0; i < Config::GRID_HEIGHT; ++i) {
						newMap[i][gridX].type = MapManager::Tile_Air;
					}
				}
			}
		}

		// Render the map
		MapManager::RenderMap(newMap);

		// Print out the information on how to use the editor
		std::string helpStr = "'E' to save file | 'Q' to close map editor\nLeft Mouse - Place tile\nRight Mouse - Erase Tile";
		std::string tileNames[] = {"1.Player", "2.Cannon", "3.Minion", "4.Boss", "5.Ground", "6.Block_1", "7.Block_2"};
		for (std::string name : tileNames) {
			helpStr += "\n" + name;
		}
		helpStr += "\nDrawing Tile " + std::to_string(drawingBlockType);
		GraphicsManager::DrawTextUI(helpStr,-1.0f,.99f, Utils::debuggerInfo.first, Utils::debuggerInfo.second);

		// Print out debugger information about the map editor
		if (Utils::isDebuggerOn) {
			f32 x, y;
			AEGfxGetCamPosition(&x, &y);

			s32 mouseX, mouseY;
			AEInputGetCursorPosition(&mouseX, &mouseY);
			float mX = 0, mY = 0;
			Utils::GetWorldMousePos(mX, mY);
			std::string debugStr = "Grid\nIndex X: " + std::to_string(gridX) + " Y: " + std::to_string(gridY)
				+ "\nPos X: " + Utils::FormatFloat(newMap[gridY][gridX].pos.x, 2) + " Y: " + Utils::FormatFloat(newMap[gridY][gridX].pos.y, 2)
				+ "\nCamera\nPos X:" + Utils::FormatFloat(x, 2) + " Y: " + Utils::FormatFloat(y, 2)
				+ "\nMouse\nScreen Pos X: " + Utils::FormatFloat(mouseX, 2) + " Y: " + Utils::FormatFloat(mouseY, 2)
				+ "\nWorld Pos X: " + Utils::FormatFloat(mX, 2) + " Y: " + Utils::FormatFloat(mY, 2);
			GraphicsManager::DrawTextUI(debugStr, 0.5f, .99f, Utils::debuggerInfo.first, Utils::debuggerInfo.second);
		}

		// Saving file
		std::string savingFilePath = "../../Assets/Map/level";
		if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
			for (int i = 1;i <= 5; i++) {
				if (AEInputCheckTriggered(AEVK_0+i)) {
					savingFilePath += std::to_string(i) + ".txt";
					SaveMapFile(savingFilePath);
				}
			}
		}

		if (AEInputCheckTriggered(AEVK_E)) {
			SaveMapFile("../../Assets/Map/level.txt"); // MIGHT HAVE SOME ERROR WHEN BUILDING DUE TO FILE PATH
		}
	}

	void SaveMapFile(std::string filePath) {
		std::ofstream output{ filePath };

		if (!output.is_open()) {
			std::cerr << "Error: Could not open file at " << filePath << std::endl;
			return;
		}

		for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
			for (int x = 0; x < Config::GRID_WIDTH; ++x) {
				output << newMap[y][x].type << " ";
			}
			output << std::endl;
		}

		isDrawingNewMap = true;
		output.close();
	}
}