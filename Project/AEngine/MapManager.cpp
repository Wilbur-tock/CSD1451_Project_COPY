#include "pch.hpp"

#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Color.hpp"

namespace MapManager {
	std::vector<std::vector<Tile>> mapLayout(Config::MAP_HEIGHT, std::vector<Tile>(Config::MAP_WIDTH));

	 void LoadMapData(const char* fileName, std::vector<std::vector<Tile>> &map, bool setBarrier) {
		std::ifstream ifs{fileName};
		if (!ifs) {
			std::cerr << "Error: Could not open map data file at " << fileName << std::endl;
			return;
		}

		// Calculate how many tiles fit on screen
		int width = Config::MAP_WIDTH;
		int height = Config::MAP_HEIGHT;
		float tileSize = static_cast<float>(Config::TILE_SIZE);

		// Offset to start from the top left corner
		float startX = -((width * tileSize) / 2.0f) + (tileSize / 2.0f);
		float startY = ((height * tileSize) / 2.0f) - (tileSize / 2.0f);

		// Initialize each Tile class data in the 2D vector
		for (int y = 0; y < Config::MAP_HEIGHT; ++y) {
			for (int x = 0; x < Config::MAP_WIDTH; ++x) {
				// Set type
				int type = 0;
				ifs >> type;
				//Set barrier around the edges
				if (setBarrier) {
					type = x == Config::MIN_GRID_X || x == Config::MAX_GRID_X || y == Config::MIN_GRID_Y || y == Config::MAX_GRID_Y ? Tile_Barrier : Tile_Air;
				}
				map[y][x].type = static_cast<TileType>(type);

				// Set position
				map[y][x].pos = { startX + (x * tileSize), startY - (y * tileSize) };
			}
		}

		ifs.close();
	}

	void RenderMap(const std::vector<std::vector<Tile>>& map) {
		float tileSize = static_cast<float>(Config::TILE_SIZE);

		for (std::vector<Tile> row : map) {
			for (Tile tile : row) {
				AEMtx33 transform = GraphicsManager::CreateTransformMatrix(tile.pos.x, tile.pos.y, 0.0f, tileSize / 2.0f, tileSize / 2.0f);

				TileType type = tile.type;
				AEGfxTexture* texture = AssetsManager::GetTexture("Assets/square.png");

				// TEMPORARY TO BE CHANGED TO TEXTURE ASSET LATER
				float red = 0.9f, g = 0.9f, b = 0.9f;

				if (type == Tile_Barrier) { // Barrier block
					red = 0, g = 1, b = 0;
				}
				else if (type == Tile_Block) {
					red = 1, g = 0, b = 0;
				}
				else if (type == Tile_Cannon) {
					red = 0, g = 0, b = 0;
				}
				else if (type == Tile_Enemy_Minion) {
					red = 0, g = 0, b = 1;
				}
				else if (type == Tile_Enemy_Boss) {
					red = 1, g = 1, b = 0;
				}

				GraphicsManager::DrawGameObject(texture, transform, Color{ red, g, b });
			}
		}
	}
}