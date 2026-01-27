#include "pch.hpp"

#include "MapEditor.hpp"
#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Utils.hpp"

using namespace MapManager;

namespace MapEditor {
	bool isDrawingNewMap = true;
	int totalLevelMap = 5;
	std::string defaultSavePath = "Assets/Map/level.txt", defaultLoadPath = "Assets/Map/default.txt",
		loadPath = "", savePath = "";

	std::vector<std::vector<Tile>> newMap(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));

	TileType defaultTile = TILE_BLOCK_GROUND;
	TileType drawingTileType = defaultTile;
	int selectedCategoryListId = 0, selectedTileListId = 0, tileId = defaultTile;
	std::vector<std::string> categories = { "Blocks", "Player", "Enemies" };
	std::vector<std::vector<std::string>> tileNames = {
		{"Ground", "Block_1", "Block_2"},
		{"Cannon", "Character"},
		{"Cannon", "Boss", "Minion"},
	};

	// If the Project/Assets/Map does not contain default.txt file 
	// it will create a new one if not just return
	static void CreateDefaultMap();
	// Ensure that the file path is going back to my project folder NOT THE BIN FOLDER
	static std::string DebugFilePath(std::string filePath) { return "../../" + filePath; }

	static void LoadNewFile(std::string filePath) {
		MapManager::ReadMapData("Assets/Map/default.txt", newMap); // Reset the map to render
		savePath = filePath;
		if (filePath == defaultLoadPath) savePath = defaultSavePath;
		loadPath = filePath;
	}

	static void SaveNewFile(std::string filePath) {
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

		output.close();
	}

	static void CreateDefaultMap() {
		std::string fileName = defaultLoadPath;
		std::ifstream ifs{ fileName };
		if (ifs) {
			//std::cout << "Contain default.txt " << fileName << std::endl;
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
		drawingTileType = defaultTile;
		CreateDefaultMap();
		LoadNewFile(defaultLoadPath);

		drawingTileType = defaultTile;
		selectedCategoryListId = 0, selectedTileListId = 0, tileId = defaultTile;
	}

	void MapEditor() {
		if (isDrawingNewMap) {
			InitMapEditor();
			isDrawingNewMap = false;
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
				newMap[gridY][gridX].type = drawingTileType;

				if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
					for (int i = 0; i < Config::GRID_WIDTH; ++i) {
						newMap[gridY][i].type = drawingTileType;
					}
				}

				if (AEInputCheckCurr(AEVK_LSHIFT) || AEInputCheckCurr(AEVK_RSHIFT)) {
					for (int i = 0; i < Config::GRID_HEIGHT; ++i) {
						newMap[i][gridX].type = drawingTileType;
					}
				}
			}
			else if (AEInputCheckCurr(AEVK_RBUTTON)) {

				newMap[gridY][gridX].type = MapManager::TILE_EMPTY;

				if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
					for (int i = 0; i < Config::GRID_WIDTH; ++i) {
						newMap[gridY][i].type = MapManager::TILE_EMPTY;
					}
				}

				if (AEInputCheckCurr(AEVK_LSHIFT) || AEInputCheckCurr(AEVK_RSHIFT)) {
					for (int i = 0; i < Config::GRID_HEIGHT; ++i) {
						newMap[i][gridX].type = MapManager::TILE_EMPTY;
					}
				}
			}
		}

		// Change drawing tile category
		if (AEInputCheckTriggered(AEVK_T)) {
			selectedCategoryListId = ++selectedCategoryListId > categories.size() - 1 ? 0 : selectedCategoryListId;

			// Reset
			selectedTileListId = 0;
			tileId = 0;
			for (int c = 0; c < selectedCategoryListId; ++c) {
				tileId += (static_cast<int>(tileNames[c].size()));
			}
			tileId++;
			drawingTileType = static_cast<MapManager::TileType>(tileId);
		}

		for (int i = 0; i < tileNames[selectedCategoryListId].size(); ++i) {
			if (AEInputCheckTriggered(static_cast<u8>(AEVK_1 + i))) {
				selectedTileListId = i;
				tileId = i + 1;

				for (int c = 0; c < selectedCategoryListId; ++c) {
					tileId += (static_cast<int>(tileNames[c].size()));
				}

				drawingTileType = static_cast<MapManager::TileType>(tileId);
			}
		}

		// Checking for input CTRL + {NUMBER} to load a new LEVEL MAP to edit
		if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
			for (int i = 1; i <= totalLevelMap; ++i) {
				if (AEInputCheckCurr(static_cast<u8>(AEVK_0 + i))) {
					// Have to load file from bin folder only
					std::string newLoadPath = "Assets/Map/level" + std::to_string(i) + ".txt";
					LoadNewFile(newLoadPath);
				}
			}
		}

		if (AEInputCheckTriggered(AEVK_E)) {
			// Save file on both BIN folder and my project folder if debugger is on
			SaveNewFile(savePath);
			if (Utils::isDebuggerOn) {
				SaveNewFile(DebugFilePath(savePath));
			}

			isDrawingNewMap = true;
		}

		// Render the map
		MapManager::RenderMap(newMap);

		// Print out the information on how to use the editor
		std::string helpStr = "Current File: "
			+ loadPath + "\nSave File: " + savePath +
			+ "\n[E] to save file\n[Q] to close map editor\n[Left Mouse] Place tile\n[Right Mouse] Erase Tile";

		// List out the type of tiles able to be drawn
		helpStr += "\n\nList of " + categories[selectedCategoryListId] + " Tiles [Key T]";
		for (int i = 0; i < tileNames[selectedCategoryListId].size(); ++i) {
			helpStr += "\n[" + std::to_string(i + 1) + "] " + tileNames[selectedCategoryListId][i];
			if (selectedTileListId == i) {
				helpStr += " [Currently Drawing] " + std::to_string(tileId);
			}
		}


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
	}
}