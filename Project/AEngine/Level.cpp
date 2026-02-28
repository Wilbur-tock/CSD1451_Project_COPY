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

Level::Level(std::string _levelName) : levelName{ _levelName } {}

void Level::Load() {
	levelMap.resize(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));
	enemies.reserve(10);

	// Read map data
	std::string filePath = "Assets/Map/" + levelName + ".txt";
	MapManager::ReadMapData(filePath, levelMap);

	AEVec2 playerCannonStartPos = { 0,0 }, enemyCannonStartPos = {0, 0};

	for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
		for (int x = 0; x < Config::GRID_WIDTH; ++x) {
			Tile& tile = levelMap[y][x];

			// Find cannon starting position
			if (tile.Type() == TILE_PLAYER_CANNON) {
				playerCannonStartPos = { tile.transform.pos.x,tile.transform.pos.y + tile.transform.size.y *0.5f};
			}

			if (tile.Type() == TILE_PLAYER_CHARACTER) {
				Character playerChar{ tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y}, CharacterType::PLAYER };
				playerCharacters.push_back(playerChar);
				tile.CharacterId(static_cast<int>(playerCharacters.size() - 1));
			}

			// Find enemy cannon starting position
			if (tile.Type() == TILE_ENEMY_CANNON) {
				enemyCannonStartPos = { tile.transform.pos.x,tile.transform.pos.y + tile.transform.size.y * 0.5f };
			}

			// Find enemy position and load enemy data into the enemies vector
			if (tile.Type() == TILE_ENEMY_MINION) {
				Character enemy{ tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y}, CharacterType::ENEMY_MINION };
				enemies.push_back(enemy);
				tile.CharacterId(static_cast<int>(enemies.size() - 1));
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
	enemyCannon = CannonManager::Cannon(enemyCannonStartPos, barrelTex, { 0,0 }, barrelHalfSize, false, false);
	enemyCannon.SetAbilityController(&PlayerManager::playerAbilityController);

	debugText = { "", Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { -1.0f, 0.99f} };

	std::cout << "[Level] Load " + levelName << '\n';
}

void Level::Initalize() {
	AEGfxSetBackgroundColor(0.92f, 0.92f, 0.95f);

	MapManager::currentMap = &levelMap;

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
	if (!isPlayerTurn) {
		// TODO: CHANGE TARGET POSITION to the player character
		activeCannon->EnemyAI(playerCannon.transform.pos);
	}
	ToggleTurn();

	// Update all Player character instance
	for (Character& playerChar : playerCharacters) {
		playerChar.Update();
	}

	// Update all enemy instance
	for (Character& enemy : enemies) {
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
		// Input R to change whose turn it is
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

		// Debug text
		f32 x, y;
		AEGfxGetCamPosition(&x, &y);

		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);
		float mX = 0, mY = 0;
		Utils::GetWorldMousePos(mX, mY);

		std::string debug = "Grid:\nIndex X: " + std::to_string(gridX) + " Y: " + std::to_string(gridY)
			+ "\n\nTile: " + GetTileName(tile.Type());

		if (tile.ContainCharacter()) {
			std::stringstream ss;
			if (tile.IsEnemy()) ss << FindEnemy(tile.CharacterId());
			else ss << FindPlayer(tile.CharacterId());
			debug += " " + ss.str();
		}

		debug += "\nPos X: " + Utils::FormatFloat(tile.transform.pos.x, 2) + " Y: " + Utils::FormatFloat(tile.transform.pos.y, 2)
			+ "\nHP: " + Utils::FormatFloat(tile.attribute.Health(), 2)
			+ "\nDamage Multiplier: " + std::to_string(tile.attribute.DamageMultiplier())
			+ "\n\nCamera " + std::to_string(CameraManager::camera.GetState())
			+ "\nPos X : " + Utils::FormatFloat(x, 2) + " Y : " + Utils::FormatFloat(y, 2)
			+ "\nMouse\nScreen Pos X: " + Utils::FormatFloat(mouseX, 2) + " Y: " + Utils::FormatFloat(mouseY, 2)
			+ "\nWorld Pos X: " + Utils::FormatFloat(mX, 2) + " Y: " + Utils::FormatFloat(mY, 2);

		debug += DebugInfo();
		debugText.textContent = debug;
		debugText.Draw();
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
	activeCannon->ResetTurn();
}

std::string Level::DebugInfo() {
	std::string info = "";

	if (Utils::isDebuggerOn) {
		for (Character& enemy : enemies) {
			if (CollisionManager::IsPointInGameObject(enemy)) {
				enemy.DebugInfo(info);
				return info;
			}
		}

		for (Character& characters : playerCharacters) {
			if (CollisionManager::IsPointInGameObject(characters)) {
				characters.DebugInfo(info);
				return info;
			}
		}
	}

	return info;
}

Character* Level::FindEnemy(int enemyId) {
	if (enemyId < 0) return nullptr;
	return &enemies[enemyId];
}

Character* Level::FindPlayer(int id) {
	if (id < 0) return nullptr;
	return &playerCharacters[id];
}