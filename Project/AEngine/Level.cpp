#include "Level.hpp"

#include "AssetsManager.hpp"
#include "MapManager.hpp"
#include "Color.hpp"
#include "Utils.hpp"
#include "CollisionManager.hpp"
#include "CameraManager.hpp"
#include "PlayerManager.hpp"
#include "LevelManager.hpp"
#include "PhysicsManager.hpp"

#include "UIManager.hpp"
#include "TextObject.hpp"

static std::string FindLevelNumber(std::string const& filePath) {
	std::string key = "level";
	size_t pos = filePath.find(key);

	if (pos != std::string::npos) {
		return filePath.substr(pos+key.size(), 1);
	}

	return "-1"; // Return -1 if "level" wasn't found
}

void Level::Load(std::string const& filePath) {
	levelMap.resize(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));
	enemies.reserve(10);

	// Read map data
	MapManager::ReadMapData(filePath, levelMap);

	AEVec2 playerCannonStartPos = { 0,0 }, enemyCannonStartPos = {0, 0};

	for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
		for (int x = 0; x < Config::GRID_WIDTH; ++x) {
			Tile& tile = levelMap[y][x];

			// Find cannon starting position
			if (tile.Type() == TILE_PLAYER_CANNON) {
				playerCannonStartPos = { tile.transform.pos.x,tile.transform.pos.y + tile.transform.size.y *0.5f};
			}

			// Find enemy cannon starting position
			if (tile.Type() == TILE_ENEMY_CANNON) {
				enemyCannonStartPos = { tile.transform.pos.x,tile.transform.pos.y + tile.transform.size.y * 0.5f };
			}

			// Find enemy position and load enemy data into the enemies vector
			if (tile.Type() == TILE_ENEMY_MINION) {
				Enemy enemy{ tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y} };
				enemies.push_back(enemy);
				tile.EnemyId(static_cast<int>(enemies.size() - 1));
			}
		}
	}

	AEGfxTexture* barrelTex = AssetsManager::GetTexture("Assets/cannonBarrel.png");
	if (!barrelTex) barrelTex = AssetsManager::GetTexture("Assets/square.png"); // fallback

	// Pick barrel half-size (these are SCALE values, not full pixels)
	// Your old barrel used {30,10} (meaning it appears ~60x20 pixels)
	AEVec2 barrelHalfSize{ 60, 20 };

	playerCannon = CannonManager::Cannon(playerCannonStartPos, barrelTex, { 0,0 }, barrelHalfSize, true, true);
	playerCannon.textureColor = Color{ 1, 1, 1, 1 };   // black barrel

	playerCannon.SetAbilityController(&PlayerManager::playerAbilityController);

	// Initialize Enemy instance
	// TODO: add a texture for the enemy cannon (flip the player cannon texture from facing right to facing left)
	enemyCannon = CannonManager::Cannon(enemyCannonStartPos, barrelTex, { 0,0 }, barrelHalfSize, false, true);
	enemyCannon.SetAbilityController(&PlayerManager::playerAbilityController);

	debugText = { "", Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { -1.0f, 0.99f} };

	levelName = "Level " + FindLevelNumber(filePath);
	std::cout << "[Level] Load " + levelName << '\n';
}

void Level::Initalize() {
	AEGfxSetBackgroundColor(0.92f, 0.92f, 0.95f);

	MapManager::currentMap = &levelMap;
	LevelManager::selectedLevel = this;

	// Always start with player first
	activeCannon = &playerCannon;

	// Set camera position to the player cannon
	CameraManager::camera.SetPosition(playerCannon.transform.pos);
	CameraManager::camera.SetState(CameraManager::CameraController::INPUT);

	std::cout << "[Level] Initialize " + levelName << std::endl;
}

void Level::Update() {
	MapManager::RenderMap(levelMap);

	activeCannon->Update();
	ToggleTurn();

	// Update all enemy instance
	for (Enemy& enemy : enemies) {
		enemy.Update();
	}

	// Rendering of assets and UI
	playerCannon.Render();
	enemyCannon.Render();

	// UI to display whose turn it is
	UIManager::Panel turnStatusPanel{ {150,50}, {1,1,1}, Utils::RectMode::RECT_TOP_CENTER};
	turnStatusPanel.Draw();

	TextObject turnText = { "PLAYER TURN", 0.75f,{0,0.75f,0}, Utils::NormalizeCoordinates(turnStatusPanel.Pos()), Utils::RectMode::RECT_CENTER};
	if (!isPlayerTurn) {
		turnText.textContent = "ENEMY TURN";
		turnText.color = { 0.75f,0,0 };
	}
	turnText.SetPos(turnStatusPanel.Pos(), turnStatusPanel.Size());
	turnText.Draw();

	if (Utils::isDebuggerOn) {
		if (AEInputCheckTriggered(AEVK_R)) {
			isPlayerTurn = !isPlayerTurn;
			SwitchCannon();
		}

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

void Level::Free() {
	LevelManager::selectedLevel = nullptr;
}

void Level::ToggleTurn() {
	if (!activeCannon->HasShot()) return;

	// Set timer to delay the transition from cannon to cannon
	if (turnTimer <= 0.0f) {
		turnTimer = Config::CANNON_SWITCH_TURN_TIMER;
	}

	if (turnTimer > 0.0f) {
		turnTimer -= PhysicsManager::g_dt;

		if (turnTimer <= 0.0f) {
			// Switch active cannon
			isPlayerTurn = !isPlayerTurn;
			SwitchCannon();

			activeCannon->ResetTurn();
			std::cout << "[Level] Toggled cannon turn" << '\n';
		}
	}
}

void Level::SwitchCannon() {
	if (isPlayerTurn) {
		activeCannon = &playerCannon;
	}
	else {
		activeCannon = &enemyCannon;
	}
	activeCannon->SetCamera(false);
}

std::string Level::DebugInfo() {
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