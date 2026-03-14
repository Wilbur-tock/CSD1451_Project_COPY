#include "Level.hpp"

#include "LevelManager.hpp"
#include "MapManager.hpp"
#include "AssetsManager.hpp"
#include "Utils.hpp"
#include "CollisionManager.hpp"
#include "CameraManager.hpp"
#include "PhysicsManager.hpp"
#include "AudioManager.hpp"
#include "GameStateManager.hpp"
#include "ParticlesManager.hpp"
#include "EffectsManager.hpp"
#include "GraphicsManager.hpp"
#include "UIManager.hpp"

AEGfxTexture* playerCannonTex = nullptr;
AEGfxTexture* enemyCannonTex = nullptr;

// UI Objects
TextObject debugText, turnText, turnCountText;
UIManager::Panel turnStatusPanel;
UIManager::Bar playerHPBar, enemyHPBar;
UIManager::Bar playerShieldBar, enemyShieldBar;

bool resultScreenInitialized = false;
UIManager::Panel resultOverlay, centerPanel;
Button resultRestartButton, resultMainMenuButton;
TextObject resultTitle, subtitle;

static void OnResultRestart() {
	GameStateManager::next = GameStateManager::GameState::RESTART;
}

static void OnResultMainMenu() {
	GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
}

void Level::InitializeUI() {
	// Initialize UI objects in level
	turnStatusPanel = { {150,75}, {1,1,1}, Utils::AnchorMode::TOP_CENTER };

	turnText = TextObject{ "", 0.8f,{0,0.75f,0}, {0,0}, Utils::AnchorMode::CENTER};
	turnText.SetPosition(turnStatusPanel.GetTransform());

	turnCountText = TextObject{ "", 0.6f,{0,0,0}, {0,0}, Utils::AnchorMode::CENTER };
	turnCountText.SetPosition(turnStatusPanel.GetTransform());
	turnCountText.SetOffsetPosition(0, -20.0f);

	AbilitiesManager::playerAbilityController.InitializeAllAbilityUI();

	float barGap = turnStatusPanel.Size().x;
	float barWidth = (Config::SCREEN_WIDTH - barGap) * 0.5f;
	float barHeight = 25.0f;
	Transform barTrans = { {0,0}, {barWidth,barHeight} };
	Utils::SetAnchoredPosition(barTrans, Utils::AnchorMode::TOP_LEFT);

	playerHPBar = UIManager::Bar{ { barTrans.pos.x, barTrans.pos.y }, barTrans.size, PlayerHPColor()};

	float rightX = barTrans.pos.x + barGap + barTrans.size.x;
	enemyHPBar = UIManager::Bar{ { rightX, barTrans.pos.y}, barTrans.size, EnemyHPColor(), UIManager::FillType::RIGHT_TO_LEFT};

	float shieldBarY = barTrans.pos.y - barHeight;
	playerShieldBar = UIManager::Bar{ { barTrans.pos.x, shieldBarY}, barTrans.size, ShieldColor() };
	enemyShieldBar = UIManager::Bar{ { rightX, shieldBarY }, barTrans.size, ShieldColor(), UIManager::FillType::RIGHT_TO_LEFT };

	// Initialize ResultsUI
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

	// -- Draw dark semi-transparent full-screen overlay --
	// Screen top-left in world-origin coords is (-SCREEN_WIDTH/2, SCREEN_HEIGHT/2)
	resultOverlay = UIManager::Panel{ {(float)Config::SCREEN_WIDTH, (float)Config::SCREEN_HEIGHT},
		Color(0.0f, 0.0f, 0.0f, 0.6f), Utils::AnchorMode::TOP_LEFT };

	centerPanel = UIManager::Panel{ {600.0f, 300.0f}, Color{1,1,1}, Utils::AnchorMode::CENTER};

	resultTitle = TextObject{ "", 2.0f, Color{1,1,1}, {0.0f, 0.0f}, Utils::AnchorMode::CENTER};
	resultTitle.SetPosition(centerPanel.GetTransform());
	// Shift title text upward inside the panel
	resultTitle.SetOffsetPosition(0, 50.0f);

	subtitle = TextObject{ "", 0.7f, Color{1,1,1, 0.9f}, {0.0f, 0.0f}, Utils::AnchorMode::CENTER};
	subtitle.SetPosition(centerPanel.GetTransform());
	subtitle.SetOffsetPosition(0, 0.2f);
}

Level::Level(int _leveId)
	: levelId{ _leveId }, levelName{ "Level " + std::to_string(levelId + 1) }, levelFileName{ "level" + std::to_string(levelId) } {
}

void Level::Load() {
	levelMap.clear();
	levelMap.resize(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));
	activeLevelMap.resize(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));

	enemies.clear();
	playerCharacters.clear();
	int reserveCount = 10;
	enemies.reserve(reserveCount);
	playerCharacters.reserve(reserveCount);

	MapManager::ReadMapData(levelFileName, levelMap);

	playerCannonTex = AssetsManager::GetTexture("Assets/cannonBarrel.png");
	enemyCannonTex = AssetsManager::GetTexture("Assets/enemyCannon.png");

	AEGfxTexture* fallbackTex = AssetsManager::GetTexture("Assets/square.png");

	if (!playerCannonTex) playerCannonTex = fallbackTex;
	if (!enemyCannonTex)  enemyCannonTex = playerCannonTex ? playerCannonTex : fallbackTex;

	debugText = { "", Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { -1.0f, 0.99f} };

	std::cout << "[Level] Load " + levelFileName << '\n';
}

void Level::Initalize() {
	status = LevelStatus::PLAYING;
	// Initialize map 
	for (int i = 0; i < Config::GRID_HEIGHT; ++i) {
		for (int j = 0; j < Config::GRID_WIDTH; ++j) {
			activeLevelMap[i][j] = levelMap[i][j];
		}
	}
	MapManager::currentMap = &activeLevelMap;

	// Reset
	playerCharacters.clear();
	enemies.clear();

	// Start of the turn should always be player turn
	activeCannon = &playerCannon;
	isPlayerTurn = true;
	switchTurnCooldown = 0.0f;

	AEVec2 playerCannonStartPos = { 0,0 }, enemyCannonStartPos = { 0,0 };

	// Initialize all the character object instance
	for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
		for (int x = 0; x < Config::GRID_WIDTH; ++x) {
			Tile& tile = activeLevelMap[y][x];

			// Set ground tile y
			if (tile.IsGround() && groundY == 0) groundY = tile.tileId.y;

			if (tile.Type() == TileType::TILE_PLAYER_CANNON) {
				playerCannonStartPos = { tile.transform.pos.x, tile.transform.pos.y + tile.transform.size.y * 0.5f };
			}
			else if (tile.Type() == TileType::TILE_ENEMY_CANNON) {
				enemyCannonStartPos = { tile.transform.pos.x, tile.transform.pos.y + tile.transform.size.y * 0.5f };
			}
			else if (tile.Type() == TileType::TILE_PLAYER_CHARACTER || tile.Type() == TileType::TILE_ENEMY_MINION || tile.Type() == TileType::TILE_ENEMY_BOSS) {
				AEVec2 characterSize{ 32.0f, 32.0f };
				Character characterObj{ tile.transform.pos, characterSize, AssetsManager::GetCharacterTexture(0)[0], {x,y}, CharacterType::PLAYER};
				int listId = 0;

				switch (tile.Type()) {
				case TileType::TILE_PLAYER_CHARACTER:
					playerCharacters.push_back(characterObj);
					listId = static_cast<int>(playerCharacters.size() - 1);
					playerCharacters[listId].id = listId;
					playerCharacters[listId].SetStats(static_cast<int>(levelDifficulty));

					//Get center tile id of the player
					SetTerritoryTiles(tile.tileId, playerCenterId, playerTiles.first, playerTiles.second);

					//SetTerritoryTiles(playerTiles, playerCenterId);
					break;

				case TileType::TILE_ENEMY_MINION:
					characterObj = { tile.transform.pos, characterSize, AssetsManager::GetCharacterTexture(1)[0], {x,y}, CharacterType::ENEMY_MINION};
					enemies.push_back(characterObj);
					listId = static_cast<int>(enemies.size() - 1);
					enemies[listId].id = listId;
					enemies[listId].SetStats(static_cast<int>(levelDifficulty));

					SetTerritoryTiles(tile.tileId, enemyCenterId, enemyTiles.first, enemyTiles.second);
					break;

				case TileType::TILE_ENEMY_BOSS:
					characterObj = { tile.transform.pos, characterSize, AssetsManager::GetCharacterTexture(2)[0], {x,y}, CharacterType::ENEMY_BOSS};
					enemies.push_back(characterObj);
					listId = static_cast<int>(enemies.size() - 1);
					enemies[listId].id = listId;
					enemies[listId].SetStats(static_cast<int>(levelDifficulty));

					SetTerritoryTiles(tile.tileId, enemyCenterId, enemyTiles.first, enemyTiles.second);

					bossId = listId;
					break;
				}

				tile.CharacterId(listId);
			}
		}
	}

	AEVec2 barrelHalfSize{ 60, 20 };
	// Initialize player cannon
	playerCannon = CannonManager::Cannon(playerCannonStartPos, playerCannonTex, { 0,0 }, barrelHalfSize, true, true);

	AbilitiesManager::AbilityController& playerController = AbilitiesManager::playerAbilityController;
	playerController.LoadAllAbilities(false);
	playerCannon.Initialize(&playerController);
	AbilitiesManager::playerAbilityController.IsSelectingAbility(true);

	// Initialize enemy cannon
	enemyCannon = CannonManager::Cannon(enemyCannonStartPos, enemyCannonTex, { 0,0 }, barrelHalfSize, false, false);

	AbilitiesManager::AbilityController& enemyController = AbilitiesManager::enemyAbilityController;
	enemyController.LoadAllAbilities(true);
	enemyCannon.Initialize(&enemyController);
	enemyCannon.SetEnemyAI(static_cast<int>(levelDifficulty));

	// Reset level flow variables
	isPlayerTurn = true;
	autoPlayOn = false;
	isSkippingEnemyTurn = false;
	switchTurnCooldown = 0.0f;

	CameraManager::camera.SetPosition(playerCannon.transform.pos);
	CameraManager::camera.SetState(CameraManager::CameraController::INPUT);

	// Randomise at the start of the level the 3 abilities that the player can choose from in the ability selector UI
	AbilitiesManager::playerAbilityController.AbilitySelectionRandomisator();

	// Initialize player and enemy shield 
	playerShield = TowerShield{ MapManager::FindTile(playerCenterId)->transform.pos };
	enemyShield = TowerShield{ MapManager::FindTile(enemyCenterId)->transform.pos };

	playerShield.SetShieldOn(10, 5);
	enemyShield.SetShieldOn(10, 5);

	InitializeUI();

	std::cout << "[Level] Initialize " + levelFileName << '\n';
}

bool prevIsAbilitySelectorOn = false;
float delayTimer = 0.0f, defaultDelayTimer = 0.5f;;

void Level::Update() {
	UpdateLevelStatus();
	DebugCommands();

	if (status == LevelStatus::PLAYING) {
		if (!isPlayerTurn && !enemyCannon.IsTargetAlive()) {
			// Find new target if current target is not alive
			enemyCannon.SetTarget(RandomCharacterObj(true));
			if (!enemyCannon.IsTargetAlive()) std::cerr << "[Level] Enemy has no target!" << std::endl;
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
	ParticleManager::particleController.Update(PhysicsManager::g_dt);

	// Need to be below DrawLevel to ensure that the Ability Selector UI is shows up infront of the level
	prevIsAbilitySelectorOn = AbilitiesManager::playerAbilityController.IsSelectingAbility();
	AbilitiesManager::playerAbilityController.AbilitySelector();

	if (AbilitiesManager::playerAbilityController.IsSelectingAbility() != prevIsAbilitySelectorOn) {
		delayTimer = defaultDelayTimer;
	}

	if (delayTimer > 0.0f) {
		delayTimer -= PhysicsManager::g_dt;
	}

	// UI
	if (isPlayerTurn) AbilitiesManager::playerAbilityController.UpdateAbilityUI();
	DrawUI();
	DebugInfo();

	// Draw result screen LAST so it renders on top of everything
	if (status == LevelStatus::PLAYER_WON || status == LevelStatus::ENEMY_WON) {
		Utils::isResultScreenOn = true;
		DrawResultsUI();
	}
	else {
		Utils::isResultScreenOn = false;
		resultScreenInitialized = false;
	}
}

void Level::Draw() {
	MapManager::DrawMap(activeLevelMap, false);

	// Drawing of assets and UI
	// Need to draw both player cannon and enemy cannon regardless of whose turn it is
	playerCannon.Draw();
	enemyCannon.Draw();

	ParticleManager::particleController.Draw();

	// Draw shield 
	playerShield.Draw();
	enemyShield.Draw();
}

void Level::Free() {
}

void Level::DrawUI() {
	// Update UI before drawing it
	if (isPlayerTurn) {
		turnText.TextContent("PLAYER TURN");
		turnText.SetColor(Color{ 0, 0.75f,0 });
	}
	else {
		turnText.TextContent("ENEMY TURN");
		turnText.SetColor(Color{ 1.0f, 0, 0 });
	}

	// UI to display whose turn it is
	turnStatusPanel.Draw();

	turnText.Draw();

	turnCountText.TextContent("Turn " + std::to_string(turnCount));
	turnCountText.Draw();

	// Draw health bar
	float currentHP = 0, maxHP = 0;
	GetCurrentHP(currentHP);
	GetMaxHP(maxHP);
	playerHPBar.Draw(currentHP, maxHP);

	GetCurrentHP(currentHP, false);
	GetMaxHP(maxHP, false);
	enemyHPBar.Draw(currentHP, maxHP);

	// Draw shield amount bar
	if (playerShield.IsShieldOn()) {
		playerShieldBar.Draw(static_cast<float>(playerShield.attribute.Shield()), static_cast<float>(playerShield.attribute.MaxShield()));
	}

	if (enemyShield.IsShieldOn()) {
		enemyShieldBar.Draw(static_cast<float>(enemyShield.attribute.Shield()), static_cast<float>(enemyShield.attribute.MaxShield()));
	}

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
	if (!activeCannon->AllProjectilesDestroyed()) return;

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
		AbilitiesManager::playerAbilityController.IsSelectingAbility(true);
		// randomise the abilities available to the player for this turn
		AbilitiesManager::playerAbilityController.AbilitySelectionRandomisator();

		NewTurn();
	}
	else {
		activeCannon = &enemyCannon;
	}
	activeCannon->ResetForNewTurn();
	AudioManager::PlaySFX(AudioManager::SFX::TurnSwitch);
	std::cout << "[Level] Switch to " << std::string(isPlayerTurn ? "PLAYER" : "ENEMY") << " turn" << '\n';
}

void Level::NewTurn() {
	++turnCount; // always increment once after enemy has shot

	playerShield.UpdateLifespan();
	enemyShield.UpdateLifespan();
}

void Level::UpdateLevelStatus() {
	status = !AbilitiesManager::playerAbilityController.IsSelectingAbility() && !isHoveringUI && delayTimer <= 0.0f && !prevIsAbilitySelectorOn
		? LevelStatus::PLAYING : LevelStatus::PAUSED;

	// Checking win conditions for both enemy and player
	float currentPlayerHP, currentEnemyHP;
	GetCurrentHP(currentPlayerHP, true);
	GetCurrentHP(currentEnemyHP, false);

	// All the enemies and boss are dead or only the boss are dead player will won
	if (EnemyBoss()) {
		if (EnemyBoss()->attribute.Health() <= 0 || currentEnemyHP <= 0) status = LevelStatus::PLAYER_WON;
	}
	else {
		if (currentEnemyHP <= 0) status = LevelStatus::PLAYER_WON;
	}
	
	if (currentPlayerHP <= 0) status = LevelStatus::ENEMY_WON;
}

void Level::DrawResultsUI() {
	resultOverlay.Draw();

	Color panelColor = (status == LevelStatus::PLAYER_WON)
		? Color(0.1f, 0.3f, 0.1f, 0.9f)
		: Color(0.3f, 0.1f, 0.1f, 0.9f);

	centerPanel.SetColor(panelColor);
	centerPanel.Draw();

	// -- Draw result title text --
	std::string resultStr = (status == LevelStatus::PLAYER_WON) ? "VICTORY!" : "DEFEAT!";
	Color textColor = (status == LevelStatus::PLAYER_WON)
		? Color(0.2f, 1.0f, 0.2f, 1.0f)
		: Color(1.0f, 0.2f, 0.2f, 1.0f);

	resultTitle.TextContent(resultStr);
	resultTitle.SetColor(textColor);
	resultTitle.Draw();

	// -- Draw subtitle text --
	std::string subStr = (status == LevelStatus::PLAYER_WON)
		? "All enemies have been defeated!"
		: "All your units have fallen!";

	subtitle.TextContent(subStr);
	subtitle.Draw();

	// -- Draw buttons --
	resultRestartButton.Update();
	resultMainMenuButton.Update();
}

void Level::SetTerritoryTiles(TileIndex const& newTileId, TileIndex& center, TileIndex& min, TileIndex& max) {
	if (min.x == 0) {
		min = newTileId;
	}
	
	// Find min/max x axis
	if (newTileId.x < min.x) {
		min = newTileId;
	}
	else if (newTileId.x > max.x) {
		max = newTileId;
	}

	// Find min/max y axis
	if (newTileId.y < min.y) {
		min = newTileId;
	}
	else if (newTileId.y > max.y) {
		max = newTileId;
	}

	// Set center id
	center = {
		min.x + ((max.x - min.x) / 2),
		min.y + ((max.y - min.y) / 2),
	};

	//std::cout << "Assign: " << newTileId << "\nCenter: " << center << "\nMin: " << min << "\nMax: " << max << '\n';
}

void Level::DebugCommands() {
	if (!Utils::isDebuggerOn) return;

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
				tile.Type(TileType::TILE_EMPTY);
				EffectsManager::effectsController.PlayTileBreak(tile.transform.pos);
			}
		}

		if (AEInputCheckTriggered(AEVK_RBUTTON)) {
			// Damage characters
			if (tile.IsCharacter()) {
				tile.TakeDamage(10);
			}
		}
	}

	// Input right click on the shield to deal damage to the shield
	if (CollisionManager::IsPointInGameObject(playerShield)) {
		if (AEInputCheckTriggered(AEVK_RBUTTON)) {
			playerShield.attribute.TakeDamage(1);
		}
	}

	if (CollisionManager::IsPointInGameObject(enemyShield)) {
		if (AEInputCheckTriggered(AEVK_RBUTTON)) {
			enemyShield.attribute.TakeDamage(1);
		}
	}

	if (isSkippingEnemyTurn) { // Skipping enemy turn
		if (!isPlayerTurn) {
			SetTurn(true);
		}

		if (autoPlayOn) autoPlayOn = false;
	}

	if (autoPlayOn) {
		AbilitiesManager::playerAbilityController.IsSelectingAbility(false);;
		if (isPlayerTurn) {
			SetTurn(false);
		}

		isSkippingEnemyTurn = false;
	}
}

UIManager::Panel debugPanel{ {350, 750}, {1,1,1, 0.8f}, Utils::AnchorMode::TOP_LEFT };

void Level::DebugInfo() {
	if (!Utils::isDebuggerOn) return;

	int gridX = 0, gridY = 0;
	Utils::GetMouseGridIndex(gridX, gridY);
	Tile& tile = *(MapManager::FindTile({ gridX, gridY }));

	// Debug text
	f32 x, y;
	AEGfxGetCamPosition(&x, &y);

	s32 mouseX, mouseY;
	AEInputGetCursorPosition(&mouseX, &mouseY);
	float mX = 0, mY = 0;
	Utils::GetWorldMousePos(mX, mY);

	std::string debug = "Shortcuts:\n[CTRL + R] Auto play " + static_cast<std::string>(autoPlayOn ? "ON" : "OFF")
		+ "\n[E] Skipping enemy turn " + static_cast<std::string>(isSkippingEnemyTurn ? "ON" : "OFF")
		+ "\n[R] Skip to enemy turn\n\nLevel:\nLevel Status: " + std::to_string(static_cast<int>(status))
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

	if (CollisionManager::IsPointInGameObject(playerShield)) {
		playerShield.DebugInfo(debug);
	}

	if (CollisionManager::IsPointInGameObject(enemyShield)) {
		enemyShield.DebugInfo(debug);
	}

	debug += info;

	debugPanel.Draw();

	debugText.TextContent(debug);
	debugText.Draw();
}

Character* Level::FindCharacterObj(Tile const& tile) {
	if (!tile.IsCharacter()) {
		std::cerr << "[Level] Tile does not contain any characters!" << '\n';
		return nullptr;
	}

	Character* character = nullptr;
	if (tile.IsEnemy()) {
		character = FindEnemyCharacterObj(tile.CharacterId());
	}
	else {
		character = FindPlayerCharacterObj(tile.CharacterId());
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
	return &characters[id];
}