#include "pch.hpp"

#include "MapEditor.hpp"
#include "MapManager.hpp"
#include "GraphicsManager.hpp"

using namespace MapManager;

namespace MapEditor {
	TileType drawingBlockType = Tile_Barrier;
	bool isDrawingNewMap = true;
	Tile newMap[Config::MAP_HEIGHT][Config::MAP_WIDTH];

	void ResetMapLayout(Tile map[Config::MAP_HEIGHT][Config::MAP_WIDTH]) {
		for (int y = 0; y < Config::MAP_HEIGHT; ++y) {
			for (int x = 0; x < Config::MAP_WIDTH; ++x) {
				map[y][x].type = x == Config::MIN_GRID_X || x == Config::MAX_GRID_X || y == Config::MIN_GRID_Y || y == Config::MAX_GRID_Y ? Tile_Barrier : Tile_Air;
			}
		}
	}

	void InitMapEditor() {
		// Reset settings
		drawingBlockType = Tile_Barrier;

		// Reset map
		ResetMapLayout(newMap);
	}

	void MapEditor() {
		if (isDrawingNewMap) {
			InitMapEditor();
			isDrawingNewMap = false;
		}

		if (AEInputCheckTriggered(AEVK_0)) {
			drawingBlockType = Tile_Barrier;
		}
		else if (AEInputCheckTriggered(AEVK_1)) {
			drawingBlockType = Tile_Air; // TEMP CHANGED LATER WHEN MORE BLOCKS TYPE ARE INTRODUCED
		}

		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		f32 camX, camY;
		AEGfxGetCamPosition(&camX, &camY);

		// Adding the offset from camera moving and add the screenWidth if the mapWidth is larger than the screenWidth
		float worldMouseX = camX + mouseX + Config::SCREEN_WIDTH;
		float worldMouseY = camY + mouseY; // If screen height is the same as screen height no offset is needed

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

		//TEMPORARY TEXTURE ONLY
		AEGfxTexture* texture = AEGfxTextureLoad("Assets/square.png");

		// Render the map
		MapManager::RenderMap(texture, newMap);

		// Print out the information on how to use the editor
		std::string helpStr = "'S' to save file, Left Mouse Click - Place tile, Right Mouse Click - Erase Tile\n 1 - Barrier, 2 - Block";
		GraphicsManager::DrawTextUI(helpStr,-1.f,-0.9f,0,0,0, 1.0f, 0.8f);

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