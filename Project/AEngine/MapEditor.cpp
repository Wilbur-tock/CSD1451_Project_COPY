#include "pch.hpp"

#include "MapEditor.hpp"
#include "MapManager.hpp"
#include "GraphicsManager.hpp"

using namespace MapManager;

namespace MapEditor {
	TileType drawingBlockType = Tile_Barrier;
	bool isDrawingNewMap = true;

	std::vector<std::vector<Tile>> newMap(Config::MAP_HEIGHT, std::vector<Tile>(Config::MAP_WIDTH));

	void InitMapEditor() {
		// Reset settings
		drawingBlockType = Tile_Barrier;

		// Reset map
		MapManager::LoadMapData("Assets/Map/default.txt", newMap);
	}

	void MapEditor() {
		if (isDrawingNewMap) {
			InitMapEditor();
			isDrawingNewMap = false;
		}

		if (AEInputCheckTriggered(AEVK_1)) {
			drawingBlockType = Tile_Barrier;
		}
		else if (AEInputCheckTriggered(AEVK_2)) {
			drawingBlockType = Tile_Block;
		}
		else if (AEInputCheckTriggered(AEVK_3)) {
			drawingBlockType = Tile_Cannon;
		}
		else if (AEInputCheckTriggered(AEVK_4)) {
			drawingBlockType = Tile_Enemy_Minion;
		}
		else if (AEInputCheckTriggered(AEVK_5)) {
			drawingBlockType = Tile_Enemy_Boss;
		}

		float worldMouseX = 0, worldMouseY = 0;
		Utils::GetWorldMousePos(worldMouseX, worldMouseY);

		int gridX = static_cast<int>(worldMouseX / Config::TILE_SIZE);
		int gridY = static_cast<int>(worldMouseY / Config::TILE_SIZE);

		//std::cout << "Mouse: (" << mouseX << ", " << mouseY << ") ";
		//std::cout << "World: (" << worldMouseX << ", " << worldMouseY << ") ";
		//std::cout << "Grid: (" << gridX << ", " << gridY << ")" << std::endl;

		if (gridX >= 0 && gridX < Config::MAP_WIDTH && gridY >= 0 && gridY < Config::MAP_HEIGHT) {
			if (gridX != Config::MIN_GRID_X && gridX != Config::MAX_GRID_X && gridY != Config::MIN_GRID_Y && gridY != Config::MAX_GRID_Y) {
				if (AEInputCheckCurr(AEVK_LBUTTON)) {
					newMap[gridY][gridX].type = drawingBlockType;
				}
				else if (AEInputCheckCurr(AEVK_RBUTTON)) {
					newMap[gridY][gridX].type = MapManager::Tile_Air;
				}
			}
		}

		// Render the map
		MapManager::RenderMap(newMap);

		// Print out the information on how to use the editor
		std::string helpStr = "'S' to save file | 'Q' to close map editor\nLeft Mouse - Place tile\nRight Mouse - Erase Tile\n1 - Barrier, 2 - Block, 3 - Cannon, 4 - Minion, 5 - Boss";
		helpStr += "\nDrawing Tile " + std::to_string(drawingBlockType);
		GraphicsManager::DrawTextUI(helpStr,-1.0f,0.95f,Color{0,0,0, 1.0f}, 0.8f);

		// Save file onto the the bin folder
		if (AEInputCheckTriggered(AEVK_S)) {
			//SaveMapFile("Assets/Map/map.txt");
			SaveMapFile("../../Assets/Map/drawnMap.txt"); // MIGHT HAVE SOME ERROR WHEN BUILDING DUE TO FILE PATH
			isDrawingNewMap = true;
		}
	}

	void SaveMapFile(const char* filePath) {
		std::ofstream output{ filePath };

		if (!output.is_open()) {
			std::cerr << "Error: Could not open file at " << filePath << std::endl;
			return;
		}

		for (int y = 0; y < Config::MAP_HEIGHT; ++y) {
			for (int x = 0; x < Config::MAP_WIDTH; ++x) {
				output << newMap[y][x].type << " ";
			}
			output << std::endl;
		}

		output.close();
		//std::cout << "File Created at: " << filePath << std::endl;
	}
}