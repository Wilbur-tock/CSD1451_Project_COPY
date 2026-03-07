#include "Level.hpp"

#include "AEInput.h"
#include "AssetsManager.hpp"
#include "MapManager.hpp"
#include "Color.hpp"
#include "Utils.hpp"
#include "CollisionManager.hpp"
#include "CameraManager.hpp"
#include "LevelManager.hpp"
#include "PhysicsManager.hpp"

#include "UIManager.hpp"
#include "TextObject.hpp"

AEGfxTexture* barrelTex = nullptr;

Level::Level(int _leveId) 
	: levelId{ _leveId }, levelName{ "Level " + std::to_string(levelId + 1) }, levelFileName { "level" + std::to_string(levelId) } {}

void Level::Load() {
	levelMap.clear();
	levelMap.resize(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));

	enemies.clear();
	playerCharacters.clear();
	int reserveCount = 5;
	enemies.reserve(reserveCount);
	playerCharacters.reserve(reserveCount);

	std::string filePath = "Assets/Map/" + levelFileName + ".txt";
	MapManager::ReadMapData(filePath, levelMap);

	barrelTex = AssetsManager::GetTexture("Assets/cannonBarrel.png");
	if (!barrelTex) barrelTex = AssetsManager::GetTexture("Assets/square.png");

	std::cout << "[Level] Load " + levelFileName << '\n';
}

void Level::Initalize() {
	MapManager::currentMap = &levelMap;

	// Start of the turn should always be player turn
	activeCannon = &playerCannon;
	isPlayerTurn = true;
	switchTurnCooldown = 0.0f;

	AEVec2 playerCannonStartPos = { 0,0 }, enemyCannonStartPos = { 0,0 };

	for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
		for (int x = 0; x < Config::GRID_WIDTH; ++x) {
			Tile& tile = levelMap[y][x];

			if (tile.Type() == TILE_PLAYER_CANNON) {
				playerCannonStartPos = { tile.transform.pos.x, tile.transform.pos.y + tile.transform.size.y * 0.5f };
			}

			if (tile.Type() == TILE_ENEMY_CANNON) {
				enemyCannonStartPos = { tile.transform.pos.x, tile.transform.pos.y + tile.transform.size.y * 0.5f };
			}

			if (tile.Type() == TILE_PLAYER_CHARACTER || tile.Type() == TILE_ENEMY_MINION || tile.Type() == TILE_ENEMY_BOSS) {
				Character characterObj{ tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y}, CharacterType::PLAYER };
				int listId = 0;

				switch (tile.Type()) {
				case TILE_PLAYER_CHARACTER:
					playerCharacters.push_back(characterObj);
					listId = static_cast<int>(playerCharacters.size() - 1);
					playerCharacters[listId].id = listId;
					break;

				case TILE_ENEMY_MINION:
					characterObj = { tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y}, CharacterType::ENEMY_MINION };
					enemies.push_back(characterObj);
					listId = static_cast<int>(enemies.size() - 1);
					enemies[listId].id = listId;
					break;

				case TILE_ENEMY_BOSS:
					characterObj = { tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y}, CharacterType::ENEMY_BOSS };
					enemies.push_back(characterObj);
					listId = static_cast<int>(enemies.size() - 1);
					enemies[listId].id = listId;
					break;
				}

				tile.CharacterId(listId);
			}
		}
	}

	AEVec2 barrelHalfSize{ 60, 20 };

	playerCannon = CannonManager::Cannon(playerCannonStartPos, barrelTex, { 0,0 }, barrelHalfSize, true, true);
	playerCannon.textureColor = Color{ 1, 1, 1, 1 };

	AbilitiesManager::AbilityController& playerController = AbilitiesManager::playerAbilityController;
	playerController.LoadAllAbilities(false);
	playerCannon.SetAbilityController(&playerController);

	enemyCannon = CannonManager::Cannon(enemyCannonStartPos, barrelTex, { 0,0 }, barrelHalfSize, false, false);
	enemyCannon.SetEnemyAI(static_cast<int>(levelDifficulty));
	AbilitiesManager::AbilityController& enemyController = AbilitiesManager::enemyAbilityController;
	enemyController.LoadAllAbilities(true);
	enemyCannon.SetAbilityController(&enemyController);

	// Reset level flow variables
	isPlayerTurn = true;
	autoPlayOn = false;
	isSkippingEnemyTurn = false;
	switchTurnCooldown = 0.0f;

	debugText = { "", Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { -1.0f, 0.99f} };

	CameraManager::camera.SetPosition(playerCannon.transform.pos);
	CameraManager::camera.SetState(CameraManager::CameraController::INPUT);

	std::cout << "[Level] Initialize " + levelFileName << std::endl;
}

bool prevIsAbilitySelectorOn = false;
bool isLevelActive = true;
float delayTimer = 0.0f, defaultDelayTimer = 0.5f;;

void Level::Update() {
	isLevelActive = !isAbilitySelectorOn && !isHoveringUI && delayTimer <= 0.0f;

	// For Debugging
	if (isSkippingEnemyTurn) { // Skipping enemy turn
		if (!isPlayerTurn) {
			SetTurn(true);
		}
	}
	else { // Skipping enemy turn is off and autoPlayOn
		if (autoPlayOn) {
			isAbilitySelectorOn = false;
			if (isPlayerTurn) {
				SetTurn(false);
			}
		}
	}

	if (isLevelActive) {
		if (!isPlayerTurn) {
			if(!enemyCannon.IsTargetAlive()) {
				// Find new target
				for (Character& playerChars : playerCharacters) {
					if (playerChars.attribute.IsActive()) {
						enemyCannon.SetTarget(&playerChars);
						break;
					}
				}
			}
		}

		if (activeCannon) {
			activeCannon->Update();
		}

		UpdateTurn();
	}

	// Update all Player character instance
	bool squatNow = isPlayerTurn && AEInputCheckCurr(AEVK_LBUTTON);

	for (Character& playerChar : playerCharacters) {
		playerChar.SetSquatAnimation(squatNow);
		playerChar.Update();
	}

	// Update all enemy instance
	for (Character& enemy : enemies) {
		enemy.Update();
	}

	Draw();

	// Need to be below DrawLevel to ensure that the Ability Selector UI is shows up infront of the level
	prevIsAbilitySelectorOn = isAbilitySelectorOn;
	AbilitiesManager::playerAbilityController.AbilitySelector(isAbilitySelectorOn);

	if (isAbilitySelectorOn != prevIsAbilitySelectorOn) {
		delayTimer = defaultDelayTimer;
	}

	if (delayTimer > 0.0f) {
		delayTimer -= PhysicsManager::g_dt;
	}

	Debugger();
}

void Level::Draw() {
	MapManager::RenderMap(levelMap, false);

	// Rendering of assets and UI
	// Need to render both player cannon and enemy cannon regardless of whose turn it is
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

void Level::Free() {
	barrelTex = nullptr;
}

void Level::UpdateTurn() {
	if (!activeCannon->HasShot()) return;

	// Set timer to delay the transition from cannon to cannon
	if (switchTurnCooldown <= 0.0f) {
		switchTurnCooldown = Config::CANNON_SWITCH_COOLDOWN_TIMER;
	}

	if (switchTurnCooldown > 0.0f) {
		switchTurnCooldown -= PhysicsManager::g_dt;

		if (switchTurnCooldown <= 0.0f) {
			// Switch active cannon
			SetTurn(!isPlayerTurn);
		}
	}
}

void Level::SetTurn(bool _isPlayerTurn) {
	isPlayerTurn = _isPlayerTurn;
	if (isPlayerTurn) {
		activeCannon = &playerCannon;
		isAbilitySelectorOn = true;
	}
	else {
		activeCannon = &enemyCannon;
	}
	activeCannon->ResetTurn();
	std::cout << "[Level] Switch to " << std::string(isPlayerTurn ? "PLAYER" : "ENEMY") << " turn" << '\n';
}

void Level::Debugger() {
	if (Utils::isDebuggerOn) {
		// Input R to change whose turn it is
		if (AEInputCheckTriggered(AEVK_R)) {
			SetTurn(!isPlayerTurn);
		}
		// Input CTRL + R to turn autoPlayOn on/off
		if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
			if (AEInputCheckTriggered(AEVK_R)) {
				autoPlayOn = !autoPlayOn;
			}
		}

		// Input E to skip enemy turn
		if (AEInputCheckTriggered(AEVK_E)) {
			isSkippingEnemyTurn = !isSkippingEnemyTurn;
		}

		int gridX = 0, gridY = 0;
		Utils::GetMouseGridIndex(gridX, gridY);

		// Input MOUSE RIGHT CLICK to remove tile blocks or damage characters during in game
		Tile& tile = *(MapManager::FindTile({ gridX, gridY }));
		if (CollisionManager::IsPointInGameObject(tile)) {
			if (AEInputCheckCurr(AEVK_RBUTTON)) {
				if (tile.IsBlocks()) {
					tile.Type(TILE_EMPTY);
				}

				// Damage characters
				if (tile.IsCharacter()) {
					Character *charPtr = FindCharacterObj(tile);
					charPtr->attribute.TakeDamage(10);
				}
			}
		}

		// Debug text
		f32 x, y;
		AEGfxGetCamPosition(&x, &y);

		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);
		float mX = 0, mY = 0;
		Utils::GetWorldMousePos(mX, mY);

		std::string debug = "Shortcuts:\n[CTRL + R] Auto play " + static_cast<std::string>(autoPlayOn ? "ON" : "OFF")
			+ "\n[E] Skipping enemy turn " + static_cast<std::string>(isSkippingEnemyTurn ? "ON" : "OFF")
			+ "\n[R] Skip to enemy turn"
			+ "\n\nTile Map\nIndex X: " + std::to_string(gridX) + " Y: " + std::to_string(gridY)
			+ "\n\nTile: " + GetTileName(tile.Type());
		// Display which character is currently at this tile type
		if (tile.IsCharacter()) {
			std::stringstream ss;
			ss << FindCharacterObj(tile);
			debug += " " + ss.str();
		}

		debug += "\nPos X: " + Utils::FormatFloat(tile.transform.pos.x, 2) + " Y: " + Utils::FormatFloat(tile.transform.pos.y, 2)
			+ "\nHP: " + Utils::FormatFloat(tile.attribute.Health(), 2)
			+ "\nDamage Multiplier: " + std::to_string(tile.attribute.DamageMultiplier())
			+ "\n\nCamera " + std::to_string(CameraManager::camera.GetState())
			+ "\nPos X : " + Utils::FormatFloat(x, 2) + " Y : " + Utils::FormatFloat(y, 2)
			+ "\nCamera State: " + CameraManager::camera.PrintState() // Camera INFO
			+ "\nZoom: " + Utils::FormatFloat(CameraManager::camera.GetZoom(), 2)
			+ "\n" + CameraManager::camera.PrintVisibleTiles()
			+ "\n\nMouse\nScreen Pos X: " + Utils::FormatFloat(mouseX, 2) + " Y: " + Utils::FormatFloat(mouseY, 2) // Mouse Info
			+ "\nWorld Pos X: " + Utils::FormatFloat(mX, 2) + " Y: " + Utils::FormatFloat(mY, 2);

		std::string info = "";

		if (Utils::isDebuggerOn) {
			// Display the character object information (All the stats, position , etc)
			for (Character& enemy : enemies) {
				if (CollisionManager::IsPointInGameObject(enemy)) {
					enemy.DebugInfo(info);
				}
			}

			for (Character& characters : playerCharacters) {
				if (CollisionManager::IsPointInGameObject(characters)) {
					characters.DebugInfo(info);
				}
			}
		}

		debug += info;

		debugText.textContent = debug;
		debugText.Draw();
	}
}

Character* Level::FindCharacterObj(Tile const& tile) {
	if (!tile.IsCharacter()) {
		std::cerr << "[Level] Tile does not contain any characters!" << '\n';
		return nullptr;
	}

	Character* character = nullptr;
	if (tile.IsEnemy()) {
		character = LevelManager::selectedLevel->FindEnemyCharacterObj(tile.CharacterId());
	}
	else {
		character = LevelManager::selectedLevel->FindPlayerCharacterObj(tile.CharacterId());
	}
	return character;
}

Character* Level::FindEnemyCharacterObj(int characterId) {
	if (characterId < 0) return nullptr;
	return &enemies[characterId];
}

Character* Level::FindPlayerCharacterObj(int characterId) {
	if (characterId < 0) return nullptr;
	return &playerCharacters[characterId];
}