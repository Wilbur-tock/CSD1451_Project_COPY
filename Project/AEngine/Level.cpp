#include "Level.hpp"

#include "AssetsManager.hpp"
#include "MapManager.hpp"
#include "Color.hpp"
#include "Utils.hpp"
#include "CollisionManager.hpp"
#include "CameraManager.hpp"
#include "PlayerManager.hpp"
#include "LevelManager.hpp"

static std::string FindLevelNumber(std::string filePath) {
	std::string key = "level";
	size_t pos = filePath.find(key);

	if (pos != std::string::npos) {
		return filePath.substr(pos+key.size(), 1);
	}

	return "-1"; // Return -1 if "level" wasn't found
}

void Level::Load(std::string filePath) {
	levelMap.resize(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));
	enemies.reserve(10);

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

			// Find enemy position and load enemy data
			if (tile.Type() == TILE_ENEMY_MINION) {
				Enemy enemy{ tile.pos, tile.size, AssetsManager::squareTexture, {x,y} };
				enemies.push_back(enemy);
				tile.EnemyId(static_cast<int>(enemies.size() - 1));
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

	playerCannon = CannonManager::Cannon(playerCannonStartPos, barrelTex, { 0,0 }, true, barrelHalfSize);
	playerCannon.textureColor = Color{ 1, 1, 1, 1 };   // black barrel

	playerCannon.SetAbilityController(&PlayerManager::playerAbilityController);

	debugText = { "", Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { -1.0f, 0.99f} };

	levelName = "Level " + FindLevelNumber(filePath);
	std::cout << "[Level] Load " + levelName << std::endl;
}

void Level::Initalize() {
	AEGfxSetBackgroundColor(0.92f, 0.92f, 0.95f);

	MapManager::currentMap = &levelMap;
	LevelManager::selectedLevel = this;

	// Set camera position to cannon
	CameraManager::camera.SetPosition(playerCannon.pos);
	CameraManager::camera.SetState(CameraManager::CameraController::INPUT);

	std::cout << "[Level] Initialize " + levelName << std::endl;
}

void Level::Update() {
	MapManager::RenderMap(levelMap);
	playerCannon.Update();

	for (Enemy& enemy : enemies) {
		enemy.Update();
	}

	if (Utils::isDebuggerOn) {
		int gridX = 0, gridY = 0;
		Utils::GetMouseGridIndex(gridX, gridY);

		// Removal of tile blocks during in game
		Tile& tile = *(MapManager::FindTile({ gridX, gridY }));
		if (CollisionManager::IsPointInGameObject(tile)) {
			if (AEInputCheckCurr(AEVK_RBUTTON) && tile.IsBlocks()) {
				tile.Type(TILE_EMPTY);
			}
		}
	}
}

std::string Level::Debugger() {
	std::string info = "";

	if (Utils::isDebuggerOn) {
		for (Enemy& enemy : enemies) {
			if (CollisionManager::IsPointInGameObject(enemy)) {
				enemy.DebugInfo(info);
				return info;
			}
		}
	}

	return info;
}

Enemy* Level::FindEnemy(int enemyId) {
	if (enemyId < 0) return nullptr;
	return &enemies[enemyId];
}