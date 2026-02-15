#include "pch.hpp"
#include "Level.hpp"
#include "AssetsManager.hpp"
#include "MapManager.hpp"
#include "Color.hpp"

void Level::Load(std::string filePath) {
	levelMap.resize(Config::GRID_HEIGHT, std::vector<MapManager::Tile>(Config::GRID_WIDTH));

	// Read map data
	MapManager::ReadMapData(filePath, levelMap);

	float minX = Config::GRID_WIDTH, minY = Config::GRID_WIDTH, maxX = 0, maxY = 0;
	// Find cannon starting position
	for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
		for (int x = 0; x < Config::GRID_WIDTH; ++x) {
			MapManager::Tile& tile = levelMap[y][x];

			// Find cannon starting position
			if (tile.type == MapManager::TILE_PLAYER_CANNON) {
				playerCannonStartPos = { tile.pos.x,tile.pos.y + tile.size.y / 2.0f };
			}

			minX = tile.IsGround() && x < minX ? x : minX;
			maxX = tile.IsGround() && x > maxX ? x : maxX;
			minY = tile.IsGround() && y < minY ? y : minY;
			maxY = tile.IsGround() && y > maxY ? y : maxY;
		}
	}

	minGround = { minX, minY };
	maxGround = { maxX, maxY };

	AEGfxTexture* barrelTex = AssetsManager::GetTexture("Assets/cannonBarrel.png");
	if (!barrelTex) barrelTex = AssetsManager::GetTexture("Assets/square.png"); // fallback

	// Pick barrel half-size (these are SCALE values, not full pixels)
	// Your old barrel used {30,10} (meaning it appears ~60x20 pixels)
	AEVec2 barrelHalfSize{ 60, 20 };

	cannon = CannonManager::Cannon(playerCannonStartPos, barrelTex, 10, true, barrelHalfSize);
	cannon.textureColor = Color{ 1, 1, 1, 1 };   // black barrel

	debugText = { "", Utils::debuggerInfo.first, Utils::debuggerInfo.second, { -1.0f, 0.99f} };
}

void Level::Init() {
	MapManager::currentMap = &levelMap;
}