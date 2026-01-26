#include "pch.hpp"

#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Color.hpp"

namespace MapManager {
	std::vector<std::vector<Tile>> mapLayout(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));

	 void LoadMapData(std::string fileName, std::vector<std::vector<Tile>> &map) {
		std::ifstream ifs{fileName};
		if (!ifs) {
			std::cerr << "Error: Could not open map data file at " << fileName << std::endl;
			return;
		}

		// Calculate how many tiles fit on screen
		int width = Config::GRID_WIDTH;
		int height = Config::GRID_HEIGHT;
		float tileSize = static_cast<float>(Config::TILE_SIZE);

		// Offset to start from the top left corner
		float startX = -((width * tileSize) / 2.0f) + (tileSize / 2.0f);
		float startY = ((height * tileSize) / 2.0f) - (tileSize / 2.0f);

		// Initialize each Tile class data in the 2D vector
		for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
			for (int x = 0; x < Config::GRID_WIDTH; ++x) {
				// Set type
				int type = 0;
				ifs >> type;
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

				// TEMPORARY TO BE CHANGED TO TEXTURE ASSET LATER
				AEGfxTexture* texture = AssetsManager::GetTexture("Assets/square.png");
				Color colors[] = { Color{ 0.9f,0.9f,0.9f }, Color{0,1,0.5f},Color{1,0,0}, Color{0,0,0}, Color{0,0,1}, Color{1,1,0}, Color{0.25f, 0.25f,0.25f}, Color{0.25f, 0.25f,0} };
				Color defaultColor = colors[tile.type];

				GraphicsManager::DrawGameObject(texture, transform, defaultColor);
			}
		}
	}
}