#include "Level.hpp"

#include "AssetsManager.hpp"
#include "MapManager.hpp"
#include "Color.hpp"
#include "Utils.hpp"
#include "CollisionManager.hpp"
#include "CameraManager.hpp"

void Level::Load(std::string filePath) {
	levelMap.resize(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));

	// Read map data
	MapManager::ReadMapData(filePath, levelMap);

	float minX = Config::GRID_WIDTH, minY = Config::GRID_WIDTH, maxX = 0, maxY = 0;
	for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
		for (int x = 0; x < Config::GRID_WIDTH; ++x) {
			Tile& tile = levelMap[y][x];

			// Find cannon starting position
			if (tile.Type() == TILE_PLAYER_CANNON) {
				playerCannonStartPos = { tile.pos.x,tile.pos.y + tile.size.y / 2.0f };
			}

			// Find ground x and y position
			minX = tile.IsGround() && x < minX ? x : minX;
			maxX = tile.IsGround() && x > maxX ? x : maxX;
			minY = tile.IsGround() && y < minY ? y : minY;
			maxY = tile.IsGround() && y > maxY ? y : maxY;

			// Find enemy position
			if (tile.Type() == TILE_ENEMY_MINION) {
				Enemy enemy{ tile.pos, tile.size, AssetsManager::squareTexture, {x,y} };
				enemies.push_back(enemy);
			}
		}
	}

	minGround = { minX, minY };
	maxGround = { maxX, maxY };

	AEGfxTexture* barrelTex = AssetsManager::GetTexture("Assets/cannonBarrel.png");
	if (!barrelTex) barrelTex = AssetsManager::GetTexture("Assets/square.png"); // fallback

	// Pick barrel half-size (these are SCALE values, not full pixels)
	// Your old barrel used {30,10} (meaning it appears ~60x20 pixels)
	AEVec2 barrelHalfSize{ 60, 20 };

	playerCannon = CannonManager::Cannon(playerCannonStartPos, barrelTex, true, barrelHalfSize);
	playerCannon.textureColor = Color{ 1, 1, 1, 1 };   // black barrel

	debugText = { "", Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { -1.0f, 0.99f} };
}

void Level::Initalize() {
	MapManager::currentMap = &levelMap;

	// Set camera position to cannon
	CameraManager::camera.SetPosition(playerCannon.pos);
	CameraManager::camera.SetState(CameraManager::CameraController::INPUT);
}

void Level::Update() {
	MapManager::RenderMap(levelMap);
	playerCannon.Update();

	for (Enemy& enemy : enemies) {
		enemy.HealthBarUI();
	}
}

std::string Level::Debugger() {
	std::string info = "";
	for (Enemy const& enemy : enemies) {
		if (Utils::isDebuggerOn) {
			if (CollisionManager::IsPointInGameObject(enemy)) {
				info = "\n\nEnemy Info: ID: x:" + std::to_string(enemy.TileId().first) + " y: " + std::to_string(enemy.TileId().second)
					+ "\nPos x : " +  Utils::FormatFloat(enemy.pos.x, 2) + " y: " + Utils::FormatFloat(enemy.pos.y, 2)
					+ "\nHP: " + std::to_string(enemy.attribute.Health())
					+ "\nDamage Multiplier: " + std::to_string(enemy.attribute.DamageMultiplier());
				return info;
			}
		}
	}

	return info;
}

Enemy* Level::FindEnemyAtGrid(int xId, int yId) {
	for (Enemy& enemy : enemies) {
		if (enemy.TileId().first == xId && enemy.TileId().second == yId) {
			return &enemy;
		}
	}
	return nullptr;
}