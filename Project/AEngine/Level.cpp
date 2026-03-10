#include "Level.hpp"

#include "AssetsManager.hpp"
#include "MapManager.hpp"
#include "Color.hpp"
#include "Utils.hpp"
#include "CollisionManager.hpp"
#include "CameraManager.hpp"
#include "LevelManager.hpp"
#include "PhysicsManager.hpp"
#include "AudioManager.hpp"

#include "UIManager.hpp"
#include "TextObject.hpp"
#include "GameStateManager.hpp"

AEGfxTexture* barrelTex = nullptr;

Level::Level(int _leveId)
	: levelId{ _leveId }, levelName{ "Level " + std::to_string(levelId + 1) }, levelFileName{ "level" + std::to_string(levelId) } {
}

void Level::Load() {
	levelMap.clear();
	levelMap.resize(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));

	enemies.clear();
	playerCharacters.clear();
	int reserveCount = 10;
	enemies.reserve(reserveCount);
	playerCharacters.reserve(reserveCount);

	std::string filePath = "Assets/Map/" + levelFileName + ".txt";
	MapManager::ReadMapData(filePath, levelMap);

	barrelTex = AssetsManager::GetTexture("Assets/cannonBarrel.png");
	if (!barrelTex) barrelTex = AssetsManager::GetTexture("Assets/square.png");

	std::cout << "[Level] Load " + levelFileName << '\n';
}

void Level::Initalize() {
	status = LevelStatus::PLAYING;
	MapManager::currentMap = &levelMap;

	// Start of the turn should always be player turn
	activeCannon = &playerCannon;
	isPlayerTurn = true;
	switchTurnCooldown = 0.0f;

	AEVec2 playerCannonStartPos = { 0,0 }, enemyCannonStartPos = { 0,0 };

	// Initialize all the character object instance
	for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
		for (int x = 0; x < Config::GRID_WIDTH; ++x) {
			Tile& tile = levelMap[y][x];

			if (tile.Type() == TILE_PLAYER_CANNON) {
				playerCannonStartPos = { tile.transform.pos.x, tile.transform.pos.y + tile.transform.size.y * 0.5f };
			}
			else if (tile.Type() == TILE_ENEMY_CANNON) {
				enemyCannonStartPos = { tile.transform.pos.x, tile.transform.pos.y + tile.transform.size.y * 0.5f };
			}
			else if (tile.Type() == TILE_PLAYER_CHARACTER || tile.Type() == TILE_ENEMY_MINION || tile.Type() == TILE_ENEMY_BOSS) {
				Character characterObj{ tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y}, CharacterType::PLAYER };
				int listId = 0;

				switch (tile.Type()) {
				case TILE_PLAYER_CHARACTER:
					playerCharacters.push_back(characterObj);
					listId = static_cast<int>(playerCharacters.size() - 1);
					playerCharacters[listId].id = listId;
					playerCharacters[listId].SetStats(static_cast<int>(levelDifficulty));
					break;

				case TILE_ENEMY_MINION:
					characterObj = { tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y}, CharacterType::ENEMY_MINION };
					enemies.push_back(characterObj);
					listId = static_cast<int>(enemies.size() - 1);
					enemies[listId].id = listId;
					enemies[listId].SetStats(static_cast<int>(levelDifficulty));
					break;

				case TILE_ENEMY_BOSS:
					characterObj = { tile.transform.pos, tile.transform.size, AssetsManager::squareTexture, {x,y}, CharacterType::ENEMY_BOSS };
					enemies.push_back(characterObj);
					listId = static_cast<int>(enemies.size() - 1);
					enemies[listId].id = listId;
					enemies[listId].SetStats(static_cast<int>(levelDifficulty));

					bossId = listId;
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
	isAbilitySelectorOn = true;

	debugText = { "", Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { -1.0f, 0.99f} };

	CameraManager::camera.SetPosition(playerCannon.transform.pos);
	CameraManager::camera.SetState(CameraManager::CameraController::INPUT);

	// Randomise at the start of the level the 3 abilities that the player can choose from in the ability selector UI
	AbilitiesManager::playerAbilityController.AbilitySelectionRandomisator();

	std::cout << "[Level] Initialize " + levelFileName << std::endl;
}

bool prevIsAbilitySelectorOn = false;
float delayTimer = 0.0f, defaultDelayTimer = 0.5f;;

void Level::Update() {
	UpdateLevelStatus();

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

	if (status == LevelStatus::PLAYING) {
		if (!isPlayerTurn && !enemyCannon.IsTargetAlive()) {
			std::cout << "[Level]" << '\n';
			// Find new target if current target is not alive
			enemyCannon.SetTarget(RandomCharacterObj(true));
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

	// UI
	if (isPlayerTurn) AbilitiesManager::playerAbilityController.AbilityUI();
	DrawUI();

	Debugger();

	// Draw result screen LAST so it renders on top of everything
	if (status == LevelStatus::PLAYER_WON || status == LevelStatus::ENEMY_WON) {
		Utils::isResultScreenOn = true;
		DrawResultScreen();
	}
	else {
		Utils::isResultScreenOn = false;
		resultScreenInitialized = false;
	}
}

void Level::Draw() {
	MapManager::DrawMap(levelMap, false);

	// Drawing of assets and UI
	// Need to draw both player cannon and enemy cannon regardless of whose turn it is
	playerCannon.Draw();
	enemyCannon.Draw();

	//DrawUI();
}

void Level::Free() {
}

void Level::DrawUI() {
	// UI to display whose turn it is
	UIManager::Panel turnStatusPanel{ {150,50}, {1,1,1}, Utils::AnchorMode::TOP_CENTER };
	turnStatusPanel.Draw();

	TextObject turnText = { "PLAYER TURN", 0.75f,{0,0.75f,0}, Utils::NormalizeCoordinates(turnStatusPanel.Pos()), Utils::AnchorMode::CENTER };
	if (!isPlayerTurn) {
		turnText.textContent = "ENEMY TURN";
		turnText.color = { 0.75f,0,0 };
	}
	turnText.SetPos(turnStatusPanel.Pos(), turnStatusPanel.Size());
	turnText.Draw();

	float colorVal = 0.9f;
	Transform barTrans = { {0,0}, {725,25} };
	Utils::SetAnchoredPosition(barTrans, Utils::AnchorMode::TOP_LEFT);

	UIManager::Bar playerHPBar{ {barTrans.pos.x, barTrans.pos.y }, barTrans.size, {0,colorVal,0} };

	float currentHP = 0, maxHP = 0;
	GetCurrentHP(currentHP);
	GetMaxHP(maxHP);
	playerHPBar.Draw(currentHP, maxHP);

	UIManager::Bar enemyHPBar{ { barTrans.pos.x + 150 + barTrans.size.x, barTrans.pos.y }, barTrans.size, {colorVal,0,0,}, UIManager::FillType::RIGHT_TO_LEFT };

	GetCurrentHP(currentHP, false);
	GetMaxHP(maxHP, false);
	enemyHPBar.Draw(currentHP, maxHP);

	activeCannon->DrawUI();
	activeCannon->DebugInfo();
}

void Level::GetCurrentHP(float& currentHP, bool isGettingPlayerHP) {
	currentHP = 0;
	if (isGettingPlayerHP) {
		for (Character& character : playerCharacters) {
			currentHP += static_cast<float>(character.attribute.Health());
		}
	}
	else {
		for (Character& character : enemies) {
			currentHP += static_cast<float>(character.attribute.Health());
		}
	}
}

void Level::GetMaxHP(float& maxHP, bool isGettingPlayerHP) {
	maxHP = 0;
	if (isGettingPlayerHP) {
		for (Character& character : playerCharacters) {
			maxHP += static_cast<float>(character.attribute.MaxHealth());
		}
	}
	else {
		for (Character& character : enemies) {
			maxHP += static_cast<float>(character.attribute.MaxHealth());
		}
	}
}

void Level::UpdateTurn() {
	if (!activeCannon->IsProjectileDestroyed()) return;

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
		// randomise the abilities available to the player for this turn
		AbilitiesManager::playerAbilityController.AbilitySelectionRandomisator();

	}
	else {
		activeCannon = &enemyCannon;
	}
	activeCannon->ResetTurn();
	AudioManager::PlaySFX(AudioManager::SFX::TurnSwitch);
	std::cout << "[Level] Switch to " << std::string(isPlayerTurn ? "PLAYER" : "ENEMY") << " turn" << '\n';
}

void Level::UpdateLevelStatus() {
	status = !isAbilitySelectorOn && !isHoveringUI && delayTimer <= 0.0f && !prevIsAbilitySelectorOn
		? LevelStatus::PLAYING : LevelStatus::PAUSED;

	// Checking win conditions for both enemy and player
	float currentPlayerHP, currentEnemyHP;
	GetCurrentHP(currentPlayerHP, true);
	GetCurrentHP(currentEnemyHP, false);

	// All the enemies and boss are dead or only the boss are dead player will won
	if (EnemyBoss()->attribute.Health() <= 0 || currentEnemyHP <= 0) status = LevelStatus::PLAYER_WON;
	if (currentPlayerHP <= 0) status = LevelStatus::ENEMY_WON;
}

static void OnResultRestart() {
	GameStateManager::next = GameStateManager::GameState::RESTART;
}

static void OnResultMainMenu() {
	GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
}

void Level::DrawResultScreen() {
	// Initialize buttons once when the result screen first appears
	if (!resultScreenInitialized) {
		resultScreenInitialized = true;

		AEGfxTexture* btnTex = AssetsManager::GetTexture("Assets/buttonTexture.png");
		if (!btnTex) btnTex = AssetsManager::squareTexture;

		float btnW = 250.0f;
		float btnH = 70.0f;
		float gap = 30.0f;

		// Coordinates use world-origin where (0,0) = screen center
		// pMeshCorner draws from top-left corner downward
		// Total button area width: btnW + gap + btnW = 530
		float totalW = btnW + gap + btnW;

		// Restart button: left side of center
		float restartX = -totalW * 0.5f;
		float restartY = -20.0f;

		resultRestartButton = Button(
			TextObject{ "RESTART" },
			Utils::AnchorMode::CENTER,
			{ restartX, restartY },
			{ btnW, btnH },
			Color(0.2f, 0.6f, 0.9f, 1.0f),
			btnTex,
			OnResultRestart
		);

		// Main menu button: right side of center
		float menuX = restartX + btnW + gap;
		float menuY = restartY;

		resultMainMenuButton = Button(
			TextObject{ "MAIN MENU" },
			Utils::AnchorMode::CENTER,
			{ menuX, menuY },
			{ btnW, btnH },
			Color(0.8f, 0.2f, 0.2f, 1.0f),
			btnTex,
			OnResultMainMenu
		);
	}

	// -- Draw dark semi-transparent full-screen overlay --
	// Screen top-left in world-origin coords is (-SCREEN_WIDTH/2, SCREEN_HEIGHT/2)
	UIManager::Panel overlay{ {(float)Config::SCREEN_WIDTH, (float)Config::SCREEN_HEIGHT},
		Color(0.0f, 0.0f, 0.0f, 0.6f), Utils::AnchorMode::TOP_LEFT };
	overlay.Draw();

	// -- Draw center panel background --
	float panelW = 600.0f;
	float panelH = 300.0f;

	Color panelColor = (status == LevelStatus::PLAYER_WON)
		? Color(0.1f, 0.3f, 0.1f, 0.9f)
		: Color(0.3f, 0.1f, 0.1f, 0.9f);

	UIManager::Panel centerPanel{ {panelW, panelH}, panelColor, Utils::AnchorMode::CENTER };
	centerPanel.Draw();

	// -- Draw result title text --
	std::string resultStr = (status == LevelStatus::PLAYER_WON) ? "VICTORY!" : "DEFEAT!";
	Color textColor = (status == LevelStatus::PLAYER_WON)
		? Color(0.2f, 1.0f, 0.2f, 1.0f)
		: Color(1.0f, 0.2f, 0.2f, 1.0f);

	TextObject resultTitle{
		resultStr,
		2.0f,
		textColor,
		{0.0f, 0.0f},
		Utils::AnchorMode::CENTER
	};
	resultTitle.SetPos(centerPanel.Pos(), centerPanel.Size());
	// Shift title text upward inside the panel
	resultTitle.pos.y += 0.15f;
	resultTitle.Draw();

	// -- Draw subtitle text --
	std::string subStr = (status == LevelStatus::PLAYER_WON)
		? "All enemies have been defeated!"
		: "All your units have fallen!";

	TextObject subtitle{
		subStr,
		0.7f,
		Color(1.0f, 1.0f, 1.0f, 0.9f),
		{0.0f, 0.0f},
		Utils::AnchorMode::CENTER
	};
	subtitle.SetPos(centerPanel.Pos(), centerPanel.Size());
	subtitle.pos.y += 0.02f;
	subtitle.Draw();

	// -- Draw buttons --
	resultRestartButton.Update();
	resultMainMenuButton.Update();
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
			}

			if (AEInputCheckTriggered(AEVK_RBUTTON)) {
				// Damage characters
				if (tile.IsCharacter()) {
					tile.TakeDamage(10);
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
			+ "\nTile: " + GetTileName(tile.Type());
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
					enemy.GetCharacterInfo(info);
				}
			}

			for (Character& characters : playerCharacters) {
				if (CollisionManager::IsPointInGameObject(characters)) {
					characters.GetCharacterInfo(info);
				}
			}
		}

		debug += info;

		UIManager::Panel{ {350, 500}, {1,1,1, 0.8f}, Utils::AnchorMode::TOP_LEFT }.Draw();

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

Character* Level::RandomCharacterObj(bool isPlayerObj) {
	std::vector<Character>& characters = isPlayerObj ? playerCharacters : enemies;

	if (characters.empty()) return nullptr;

	int id = Utils::RandomValue(0, static_cast<int>(characters.size() - 1));
	std::cout << "Found " << characters[id].TypeName() << '\n';
	return &characters[id];
}