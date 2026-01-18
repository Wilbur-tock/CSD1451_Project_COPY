#include "pch.hpp"

#include "MapEditor.hpp"
#include "MapManager.hpp"
#include "GraphicsManager.hpp"

using namespace MapManager;

namespace MapEditor {
	TileType drawingBlockType = Tile_Block;
	bool isDrawingNewMap = true;
	Tile newMap[Config::mapHeight][Config::mapWidth];

	void ResetMapLayout(Tile map[Config::mapHeight][Config::mapWidth]) {
		for (int y = 0; y < Config::mapHeight; ++y) {
			for (int x = 0; x < Config::mapWidth; ++x) {
				map[y][x].type = Tile_Air;
			}
		}
	}

	void InitMapEditor() {
		// Reset settings
		drawingBlockType = Tile_Block;

		// Reset map
		ResetMapLayout(newMap);
	}

	void MapEditor() {
		if (isDrawingNewMap) {
			InitMapEditor();
			isDrawingNewMap = false;
		}

		if (AEInputCheckTriggered(AEVK_0)) {
			drawingBlockType = Tile_Block;
		}
		else if (AEInputCheckTriggered(AEVK_1)) {
			drawingBlockType = Tile_Air; // TEMP CHANGED LATER WHEN MORE BLOCKS TYPE ARE INTRODUCED
		}

		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		// Convert Screen Space to World Space
		// AE Screen (0,0) is Top-Left, World (0,0) is Center
		//float worldX = (float)mouseX - (Config::screenWidth / 2.0f);
		//float worldY = (Config::screenHeight / 2.0f) - (float)mouseY;

		// Convert World Space to Grid Index
		int gridX = mouseX / Config::tileSize;
		int gridY = mouseY / Config::tileSize;

		// Bounds Checking & Input
		if (gridX >= 0 && gridX < Config::mapWidth && gridY >= 0 && gridY < Config::mapHeight) {
			if (AEInputCheckCurr(VK_LBUTTON)) {
				newMap[gridY][gridX].type = drawingBlockType;
				std::cout << gridY << " " << gridX << std::endl;
			}
			else if (AEInputCheckCurr(VK_RBUTTON)) {
				newMap[gridY][gridX].type = MapManager::Tile_Air;
			}
		}

		//TEMPORARY TEXTURE ONLY
		AEGfxTexture* texture = AEGfxTextureLoad("Assets/square.png");
		MapManager::RenderMap(texture, newMap);

		// Save file onto the the bin folder
		if (AEInputCheckTriggered(AEVK_S)) {
			SaveMapFile("Assets/Map/map.txt");
			isDrawingNewMap = true;
		}
	}

	void SaveMapFile(const char* filePath) {
		std::ofstream output{ filePath };

		if (!output.is_open()) {
			std::cerr << "Error: Could not open file at " << filePath << std::endl;
			return;
		}

		for (int y = 0; y < Config::mapHeight; ++y) {
			for (int x = 0; x < Config::mapWidth; ++x) {
				output << newMap[y][x].type << " ";
			}
			output << std::endl;
		}

		output.close();
		//std::cout << "File Created Successfully at: " << filePath << std::endl;
	}
}