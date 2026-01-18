#include "pch.hpp"

#include "MapManager.hpp"
#include "GraphicsManager.hpp"

namespace MapManager {

	Tile tileMapLayout[Config::mapHeight][Config::mapWidth];

	void RenderMap(AEGfxTexture* tileTexture, Tile const mapLayout[Config::mapHeight][Config::mapWidth]) {
		// Calculate how many tiles fit on screen
		int width = Config::mapWidth;
		int height = Config::mapHeight;
		float tileSize = static_cast<float>(Config::tileSize);


		// Offset to center the grid or start from a corner
		float startX = -(static_cast<float>(Config::screenWidth) / 2.0f) + (tileSize / 2.0f);
		float startY = (static_cast<float>(Config::screenHeight) / 2.0f) - (tileSize / 2.0f);

		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				float posX = startX + (x * tileSize);
				float posY = startY - (y * tileSize);

				AEMtx33 transform = GraphicsManager::CreateTransformMatrix(posX, posY, 0.0f, tileSize / 2.0f, tileSize / 2.0f);

				// TEMPORARY TO BE CHANGED TO TEXTURE ASSET LATER
				float red = 1.0f, g = 0, b = 0;

				if (mapLayout[y][x].type == 1) {
					red = 0, g = 1, b = 0;
					//std::cout << "Changed Grid:" << y << " " << x << std::endl;
				}

				GraphicsManager::DrawGameObject(tileTexture, transform, red, g, b, 1.0f);
			}
		}
	}
}