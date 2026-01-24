#include "pch.hpp"

#include "MapManager.hpp"

namespace MapManager {

	Tile mapLayout[Config::MAP_HEIGHT][Config::MAP_WIDTH];

	void LoadMapData(const char* fileName) {
		std::ifstream ifs{fileName};
		if (!ifs) {
			std::cerr << "Error: Could not open map data file at " << fileName << std::endl;
			return;
		}

		for (int y = 0; y < Config::MAP_HEIGHT; ++y) {
			for (int x = 0; x < Config::MAP_WIDTH; ++x) {
				int type = 0;
				ifs >> type;
				mapLayout[y][x].type = static_cast<TileType>(type);
			}
		}

		ifs.close();
	}

	void LoadAllMapData() {
		LoadMapData("Assets/Map/level1.txt");
	}

	void RenderMap(AEGfxTexture* tileTexture, Tile const map[Config::MAP_HEIGHT][Config::MAP_WIDTH]) {
		// Calculate how many tiles fit on screen
		int width = Config::MAP_WIDTH;
		int height = Config::MAP_HEIGHT;
		float tileSize = static_cast<float>(Config::TILE_SIZE);

		// Offset to center the grid or start from a corner
		float startX = -((width * tileSize) / 2.0f) + (tileSize / 2.0f);
		float startY = ((height * tileSize) / 2.0f) - (tileSize / 2.0f);

		//DebugPrint::Print(startX, startY);

		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				float posX = startX + (x * tileSize);
				float posY = startY - (y * tileSize);

				AEMtx33 transform = GraphicsManager::CreateTransformMatrix(posX, posY, 0.0f, tileSize / 2.0f, tileSize / 2.0f);

				TileType type = map[y][x].type;

				// TEMPORARY TO BE CHANGED TO TEXTURE ASSET LATER
				float red = 0.9f, g = 0.9f, b = 0.9f;

				if (type == Tile_Barrier) { // Barrier block
					g = 1;
				}
				else if (type == Tile_Block) {
					red = 1;
				}

				GraphicsManager::DrawGameObject(tileTexture, transform, red, g, b);
			}
		}
	}
}