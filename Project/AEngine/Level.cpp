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
	int reserveCount = 5;
	enemies.reserve(reserveCount);
	playerCharacters.reserve(reserveCount);

	// Read the map data
	std::string filePath = "Assets/Map/" + levelName + ".txt";
	MapManager::ReadMapData(filePath, levelMap);

	AEVec2 playerCannonStartPos = { 0,0 }, enemyCannonStartPos = {0, 0};

	// Loop through each tile block to find the position of the character objects, cannons, etc
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

			// Find all instance of player and enemy character objects
			if (tile.Type() == TILE_PLAYER_CHARACTER || tile.Type() == TILE_ENEMY_MINION || tile.Type() == TILE_ENEMY_BOSS) {
				Character characterObj{ tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y}, CharacterType::PLAYER };
				int listId = 0;
				switch (tile.Type()) {
				case TILE_PLAYER_CHARACTER:
					playerCharacters.push_back(characterObj);
					listId = static_cast<int>(playerCharacters.size() - 1);
					playerCharacters[listId].id = listId; // assign the character id to the character component itself
					break;
				case TILE_ENEMY_MINION:
					characterObj = { tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y}, CharacterType::ENEMY_MINION };
					enemies.push_back(characterObj);
					listId = static_cast<int>(enemies.size() - 1);
					enemies[listId].id = listId; // assign the character id to the character component itself
					break;
				case TILE_ENEMY_BOSS:
					characterObj = { tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y}, CharacterType::ENEMY_BOSS };
					enemies.push_back(characterObj);
					listId = static_cast<int>(enemies.size() - 1);
					enemies[listId].id = listId; // assign the character id to the character component itself
					break;
				}

				tile.CharacterId(listId); // assign the character id to the tile component
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

	AbilitiesManager::AbilityController& playerController = AbilitiesManager::playerAbilityController;
	playerController.LoadAllAbilities();
	playerCannon.SetAbilityController(&playerController);

	// Initialize Enemy instance
	// TODO: add a texture for the enemy cannon (flip the player cannon texture from facing right to facing left)
	enemyCannon = CannonManager::Cannon(enemyCannonStartPos, barrelTex, { 0,0 }, barrelHalfSize, false, false);
	// Set enemy ability controller
	AbilitiesManager::AbilityController& enemyController = AbilitiesManager::enemyAbilityController;
	enemyController.LoadAllAbilities();
	enemyCannon.SetAbilityController(&enemyController);

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

	if (autoPlayOn && isPlayerTurn) {
		isPlayerTurn = false;
		SwitchCannon();
	}

	activeCannon->Update();
	if (!isPlayerTurn) {
		AEVec2 targetPos = { 0,0 };
		if (playerCharacters.size() > 0) targetPos = playerCharacters[0].transform.pos;
		activeCannon->EnemyAI(targetPos);
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

	Draw();

	if (Utils::isDebuggerOn) {
		// Input R to change whose turn it is
		if (AEInputCheckTriggered(AEVK_R)) {
			isPlayerTurn = !isPlayerTurn;
			SwitchCannon();
		}
		// Input CTRL + R to turn autoPlayOn on/off
		if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
			if (AEInputCheckTriggered(AEVK_R)) {
				autoPlayOn = !autoPlayOn;
			}
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

		std::string debug = "Auto play " + static_cast<std::string>(autoPlayOn ? "ON" : "OFF")
			+ "\nGrid:\nIndex X: " + std::to_string(gridX) + " Y: " + std::to_string(gridY)
			+ "\n\nTile: " + GetTileName(tile.Type());
		// Display which character is currently at this tile type
		if (tile.ContainCharacter()) {
			std::stringstream ss;
			if (tile.IsEnemy()) ss << FindEnemyCharacterObj(tile.CharacterId());
			else ss << FindPlayerCharacterObj(tile.CharacterId());
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

void Level::Draw() {
	// Rendering of assets and UI
	playerCannon.Render();
	enemyCannon.Render();

	// UI to display whose turn it is
	UIManager::Panel turnStatusPanel{ {150,50}, {1,1,1}, Utils::RectMode::RECT_TOP_CENTER };
	turnStatusPanel.Draw();

	TextObject turnText = { "PLAYER TURN", 0.75f,{0,0.75f,0}, Utils::NormalizeCoordinates(turnStatusPanel.Pos()), Utils::RectMode::RECT_CENTER };
	if (!isPlayerTurn) {
		turnText.textContent = "ENEMY TURN";
		turnText.color = { 0.75f,0,0 };
	}
	turnText.SetPos(turnStatusPanel.Pos(), turnStatusPanel.Size());
	turnText.Draw();
}

void Level::Free() {}

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
	std::cout << "[Level] Toggle to " << std::string(isPlayerTurn ? "PLAYER" : "ENEMY") << " turn" << '\n';
}

std::string Level::DebugInfo() {
	std::string info = "";

	if (Utils::isDebuggerOn) {
		// Display the character object information (All the stats, position , etc)
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

Character* Level::FindEnemyCharacterObj(int characterId) {
	if (characterId < 0) return nullptr;
	return &enemies[characterId];
}

Character* Level::FindPlayerCharacterObj(int characterId) {
	if (characterId < 0) return nullptr;
	return &playerCharacters[characterId];
}