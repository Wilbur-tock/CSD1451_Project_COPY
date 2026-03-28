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
#include "GameTrackingManager.hpp"
#include "BgAnimation.hpp"
#include "ResultsUI.hpp"

// UI Objects

// Debug Panel
UIManager::PanelText debugPanel, cannonDebugPanel;

TextObject levelText, turnText, roundCountText;
UIManager::Panel turnStatusPanel;

struct LevelStatsUI {
	UIManager::Panel barBG;
	UIManager::Bar hpBar, shieldBar;
	std::vector<UIManager::IconWithText> abilityModifierIcons;

	// isLeft = true, is player stats
	// bgWidth = the width of the hp and shield bar
	void Initialize(bool isLeft, float bgWidth) {
		UIManager::FillType fillType = UIManager::FillType::LEFT_TO_RIGHT;
		Utils::AnchorMode barBGAnchorMode = Utils::AnchorMode::TOP_LEFT;
		Color hpColor = PlayerHPColor();

		if (!isLeft) {
			fillType = UIManager::FillType::RIGHT_TO_LEFT;
			barBGAnchorMode = Utils::AnchorMode::TOP_RIGHT;
			hpColor = EnemyHPColor();
		}

		// bar width and height
		Transform barTransform = { {0,0}, {bgWidth, 25.0f} };
		Utils::SetAnchoredPosition(barTransform, barBGAnchorMode);
		// BG panel of the both bar, so get the height of the both bars + 10.0f as a gap
		AEVec2 barBGSize{ barTransform.size.x, (barTransform.size.y * 2.0f) + 10.0f };
		Color barBGColor = { 0.9f, 0.9f, 0.9f };

		float shieldBarY = barTransform.pos.y - barTransform.size.y;

		// Initialize the barBG and hp and shield bar
		barBG = UIManager::Panel{ barBGSize, barBGColor, barBGAnchorMode };
		hpBar = UIManager::Bar{ { barTransform.pos.x, barTransform.pos.y }, barTransform.size, hpColor, fillType };
		shieldBar = UIManager::Bar{ { barTransform.pos.x, shieldBarY}, barTransform.size, ShieldColor(), fillType };

		int amCount = Config::MAX_ABILITY_MODIFIER_COUNT;
		float amPanelGaps = 5.0f;
		AEVec2 amPanelSize{ 50.0f, 50.0f };

		// Calculate offset position from the main panel
		float amXOffset = (turnStatusPanel.GetTransform().size.x * 0.5f) + (amPanelSize.x * 0.5f);
		float amYOffset = -barBGSize.y;

		float gapBetweenBorder = 7.5f;

		// Initialize ability modifier UI objects
		abilityModifierIcons.reserve(amCount);

		for (int i = 0; i < amCount; ++i) {
			// Calculate each gap between each panel object [PANEL][GAP][PANEL]
			float totalWidthAndGaps = (amPanelGaps * i) + (amPanelSize.x * i);

			// Calculate if the icons are on the player/enemy side
			float xOffset = isLeft ? -amXOffset - totalWidthAndGaps : amXOffset + totalWidthAndGaps;

			AEVec2 iconSize{ amPanelSize.x - (gapBetweenBorder * 2.0f), amPanelSize.y - (gapBetweenBorder * 2.0f) };

			UIManager::IconWithText amIcon;
			amIcon.Initialize(Transform{ {0,0}, {amPanelSize} }, Transform{ {0,0},{iconSize} });

			// Add position offset to ensure that the icons are position correctly 
			// position below the bars bg
			amIcon.OffsetPosition(xOffset, amYOffset, gapBetweenBorder);

			abilityModifierIcons.push_back(amIcon);
		}
	}

	void Draw(float currentHP, float maxHP, bool isShieldOn, float currentShield, float maxShield, std::vector<Ability> const& abilityModifiers) {
		hpBar.Draw(currentHP, maxHP);
		if (isShieldOn) {
			shieldBar.Draw(currentShield, maxShield);
		}

		for (int i = 0; i < abilityModifiers.size(); ++i) {
			Ability const& ability = abilityModifiers[i];

			// decide colors
			Color amPanelColor{ 0, 0.9f, 0 };
			if (!ability.GetStatus().isPositive) {
				amPanelColor.SetColor(0.9f, 0, 0);
			}

			abilityModifierIcons[i].Draw(amPanelColor, ability.GetTexture(), std::to_string(ability.GetStatus().rounds));
		}
	}
};

// Store cannon texture here
AEGfxTexture* playerCannonTex = nullptr;
AEGfxTexture* enemyCannonTex = nullptr;

LevelStatsUI playerStatsUI, enemyStatsUI; // UI object to display the overall stats of the player and enemy
UIManager::PopupPanel notificationPanel; // panel to display whose turn it is disappear after a while

void Level::InitializeUI() {
	debugPanel = UIManager::PanelText{
		UIManager::Panel{ {350, 750}, {1,1,1, 0.8f}, Utils::AnchorMode::TOP_LEFT },
		TextObject{"", Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { -1.0f, 0.99f}}
	};

	cannonDebugPanel = UIManager::PanelText{
		UIManager::Panel{ {400, 750}, {1,1,1, 0.8f}, Utils::AnchorMode::TOP_RIGHT },
		TextObject{"", Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { -1.0f, 0.99f} }
	};

	// Initialize UI objects in level
	turnStatusPanel = { {150,75}, {1,1,1}, Utils::AnchorMode::TOP_CENTER };

	levelText = TextObject{ levelName, 0.6f, {0,0,0}, { {0,0}, {0,0} }, Utils::AnchorMode::CENTER };
	levelText.SetPosition(turnStatusPanel.GetTransform());
	levelText.SetOffsetPosition(0, 20.0f);

	turnText = TextObject{ "", 0.8f,{0,0.75f,0}, { {0,0}, {0,0} }, Utils::AnchorMode::CENTER };
	turnText.SetPosition(turnStatusPanel.GetTransform());

	roundCountText = TextObject{ "", 0.6f,{0,0,0}, { {0,0}, {0,0} }, Utils::AnchorMode::CENTER };
	roundCountText.SetPosition(turnStatusPanel.GetTransform());
	roundCountText.SetOffsetPosition(0, -20.0f);

	activeCannon->AbilityController()->InitializeAllAbilityUI();

	// Initialize the stats of player and enemy UI
	float bgWidth = (Config::SCREEN_WIDTH - turnStatusPanel.GetTransform().size.x) * 0.5f;
	playerStatsUI.Initialize(true, bgWidth);
	enemyStatsUI.Initialize(false, bgWidth);

	notificationPanel = UIManager::PopupPanel{
		true,
		UIManager::Panel{{250,75}},
		TextObject{"", 0.7f, Color{0.0f,0.1f,0.7f}, Transform{ {0,0}, {0,0} }, Utils::AnchorMode::CENTER }
	};
	notificationPanel.AddOffsetPosition({ 0,300 });

	ResultsUI::Intialize();
}

Level::Level(int _leveId)
	: levelId{ _leveId }, levelName{ levelId == 0 ? "Tutorial" : "Level " + std::to_string(levelId)},
	levelFileName{ "level" + std::to_string(levelId) } {
}

void Level::Load() {
	levelMap.clear();
	levelMap.resize(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));
	activeLevelMap.resize(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));

	// Load map data
	MapManager::ReadMapData(levelFileName, levelMap);

	// Sky base texture - drawn behind everything else each frame.
	// PLACEHOLDER: provide Assets/level_sky.png (1600x900 static sky image).
	bgTexture = AssetsManager::GetTexture("Assets/level_sky.png");

	// Load cloud layer textures via BgAnimation.
	// Shares the same PNGs as MainMenu and LevelSelect - no extra files needed.
	BgAnimation::Clouds::Load();

	playerCannonTex = AssetsManager::GetTexture("Assets/cannonBarrel.png");
	enemyCannonTex = AssetsManager::GetTexture("Assets/enemyCannon.png");

	AEGfxTexture* fallbackTex = AssetsManager::GetTexture("Assets/square.png");

	if (!playerCannonTex) playerCannonTex = fallbackTex;
	if (!enemyCannonTex)  enemyCannonTex = playerCannonTex ? playerCannonTex : fallbackTex;

	std::cout << "[Level] Load " + levelFileName << '\n';
}

void Level::Initalize() {
	// Reset cloud scroll offsets so re-entering the level always starts cleanly.
	// Called here (not in Load) so a restart also resets the animation.
	BgAnimation::Clouds::Init();

	GameTrackingManager::gameData.LevelStart(levelId);

	status = LevelStatus::PLAYING;
	hasProcessedVictory = false;

	// Initialize active map 
	for (int i = 0; i < Config::GRID_HEIGHT; ++i) {
		for (int j = 0; j < Config::GRID_WIDTH; ++j) {
			activeLevelMap[i][j] = levelMap[i][j];
		}
	}
	MapManager::currentMap = &activeLevelMap;

	// Reset
	playerCharacters.clear();
	enemies.clear();
	decayingTiles.clear();

	// reverse tiles
	int reserveCount = 10;
	enemies.reserve(reserveCount);
	playerCharacters.reserve(reserveCount);
	decayingTiles.reserve(64);

	// Initialize variables
	activeCannon = isPlayerStarting ? &playerCannon : &enemyCannon;
	isPlayerTurn = isPlayerStarting;
	roundCount = 0;
	// Reset debugging back to false
	autoPlayOn = false;
	noEnemyTurn = false;
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
				Character characterObj{ tile.transform.pos, characterSize, AssetsManager::GetCharacterTexture(0)[0], {x,y}, CharacterType::PLAYER };
				int listId = 0;

				switch (tile.Type()) {
				case TileType::TILE_PLAYER_CHARACTER:
					playerCharacters.push_back(characterObj);
					listId = static_cast<int>(playerCharacters.size() - 1);
					playerCharacters[listId].id = listId;
					playerCharacters[listId].SetStats(static_cast<int>(levelDifficulty));

					//Get center tile id of the player
					SetTerritoryIndex(tile.tileId, playerTerritory.centerId, playerTerritory.minId, playerTerritory.maxId);

					//SetTerritoryTiles(playerTiles, playerCenterId);
					break;

				case TileType::TILE_ENEMY_MINION:
					characterObj = { tile.transform.pos, characterSize, AssetsManager::GetCharacterTexture(1)[0], {x,y}, CharacterType::ENEMY_MINION };
					enemies.push_back(characterObj);
					listId = static_cast<int>(enemies.size() - 1);
					enemies[listId].id = listId;
					enemies[listId].SetStats(static_cast<int>(levelDifficulty));

					SetTerritoryIndex(tile.tileId, enemyTerritory.centerId, enemyTerritory.minId, enemyTerritory.maxId);
					break;

				case TileType::TILE_ENEMY_BOSS:
					characterObj = { tile.transform.pos, characterSize, AssetsManager::GetCharacterTexture(2)[0], {x,y}, CharacterType::ENEMY_BOSS };
					enemies.push_back(characterObj);
					listId = static_cast<int>(enemies.size() - 1);
					enemies[listId].id = listId;
					enemies[listId].SetStats(static_cast<int>(levelDifficulty));

					SetTerritoryIndex(tile.tileId, enemyTerritory.centerId, enemyTerritory.minId, enemyTerritory.maxId);

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
	playerController.LoadAllAbilities(true);
	playerController.AbilitySelectionRandomiser();
	playerCannon.Initialize(&playerController);
	AbilitiesManager::playerAbilityController.IsSelectingAbility(true);

	// Initialize enemy cannon
	enemyCannon = CannonManager::Cannon(enemyCannonStartPos, enemyCannonTex, { 0,0 }, barrelHalfSize, false, false);

	AbilitiesManager::AbilityController& enemyController = AbilitiesManager::enemyAbilityController;
	enemyController.LoadAllAbilities(false);
	enemyController.SetAIAbilitySelection(static_cast<int>(levelDifficulty));
	enemyCannon.Initialize(&enemyController);
	enemyCannon.SetEnemyAI(static_cast<int>(levelDifficulty));

	// Reset camera
	CameraManager::camera.SetPosition(playerCannon.transform.pos);
	CameraManager::camera.SetState(CameraManager::CameraController::INPUT);

	// Initialize player and enemy shield 
	playerShield = TowerShield{ MapManager::FindTile(playerTerritory.centerId)->transform.pos, &playerController };
	enemyShield = TowerShield{ MapManager::FindTile(enemyTerritory.centerId)->transform.pos, &enemyController };

	// Initialize Territory tiles
	SetOriginalTerritoryTiles(playerTerritory);

	InitializeUI();

	SwitchTurn(isPlayerStarting); // called last

	std::cout << "[Level] Initialize " + levelFileName << '\n';
}

bool prevIsAbilitySelectorOn = false;
float delayTimer = 0.0f, defaultDelayTimer = 0.5f;;

void Level::Update() {
	// Advance cloud scroll for this frame.
	// PhysicsManager::g_dt is used for consistency with all other per-frame motion here.
	BgAnimation::Clouds::Update(PhysicsManager::g_dt);

	UpdateLevelStatus();
	DebugCommands();

	if (status == LevelStatus::PLAYING) {
		// Find new target if current target is not alive
		if (!isPlayerTurn && !enemyCannon.IsTargetAlive()) {
			enemyCannon.SetTarget(RandomCharacterObj(true));
			if (!enemyCannon.IsTargetAlive()) std::cerr << "[Level] Enemy has no target!" << std::endl;
		}

		if (activeCannon) {
			if (isPlayerTurn) {
				activeCannon->CanCannonShoot(!isHoveringOverInventoryUI); // if mouse is hovering over inventory UI disable cannon left click shooting
			}
			activeCannon->Update();
		}

		UpdateTurn();
	}

	Draw();
	for (Character& playerChar : playerCharacters) {
		playerChar.Update();
		playerChar.Draw();
	}

	// Update all enemy instance
	for (Character& enemy : enemies) {
		enemy.Update();
		enemy.Draw();
	}

	ParticleManager::particleController.Update(PhysicsManager::g_dt);

	// Need to be below DrawLevel to ensure that the Ability Selector UI is shows up infront of the level
	prevIsAbilitySelectorOn = AbilitiesManager::playerAbilityController.IsSelectingAbility();
	AbilitiesManager::playerAbilityController.UpdateAbilitySelector();

	if (AbilitiesManager::playerAbilityController.IsSelectingAbility() != prevIsAbilitySelectorOn) {
		delayTimer = defaultDelayTimer;
	}

	if (delayTimer > 0.0f) {
		delayTimer -= PhysicsManager::g_dt;
	}

	// UI
	activeCannon->AbilityController()->DrawAbilityInventoryUI();
	UpdateUI();
	DrawUI();
	DebugInfo();

	// Draw result screen LAST so it renders on top of everything
	ResultsUI::Update(status);
}

void Level::UpdateUI() {
	notificationPanel.Update();
}

void Level::Draw() {
	// Sky base: drawn first so everything else renders on top of it.
	// DrawScreenOverlay keeps it screen-fixed even as the world camera pans/zooms.
	if (bgTexture) {
		GraphicsManager::DrawScreenOverlay(
			bgTexture,
			GraphicsManager::CreateTransformMatrix(
				-Config::SCREEN_WIDTH / 2.0f,
				Config::SCREEN_HEIGHT / 2.0f,
				Config::SCREEN_WIDTH,
				Config::SCREEN_HEIGHT),
			Color(1.0f, 1.0f, 1.0f, 1.0f)
		);
	}

	// Animated cloud layers drawn on top of the sky base, behind game content.
	// BgAnimation::Clouds uses DrawScreenOverlay internally so the layers stay
	// screen-fixed even as the world camera pans and zooms during gameplay.
	BgAnimation::Clouds::Draw();

	// Tile map and game objects drawn on top of the background
	//GraphicsManager::DrawGameObject(AssetsManager::GetTexture("Assets/level_bg.png"), GraphicsManager::CreateTransformMatrix(0, 0, Config::WORLD_MAX_X, Config::WORLD_MAX_Y), Color(1,1,1));
	MapManager::DrawMap(activeLevelMap, false);

	// Drawing of assets and UI
	// Need to draw both player cannon and enemy cannon regardless of whose turn it is
	playerCannon.Draw();
	enemyCannon.Draw();

	// Draw shield 
	playerShield.Draw();
	enemyShield.Draw();

	ParticleManager::particleController.Draw();
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
	levelText.Draw();

	// Display how many numbers of round 
	roundCountText.TextContent("Round " + std::to_string(roundCount));
	roundCountText.Draw();

	// Draw stats of player and enemy
	playerStatsUI.barBG.Draw();
	enemyStatsUI.barBG.Draw();

	float currentHP = 0, maxHP = 0;
	GetCurrentHP(currentHP);
	GetMaxHP(maxHP);
	playerStatsUI.Draw(currentHP, maxHP, playerShield.IsShieldOn(), static_cast<float>(playerShield.attribute.Shield()), static_cast<float>(playerShield.attribute.MaxShield()),
		playerCannon.AbilityController()->GetAbilityModifiers());

	GetCurrentHP(currentHP, false);
	GetMaxHP(maxHP, false);
	enemyStatsUI.Draw(currentHP, maxHP, enemyShield.IsShieldOn(), static_cast<float>(enemyShield.attribute.Shield()), static_cast<float>(enemyShield.attribute.MaxShield()),
		enemyCannon.AbilityController()->GetAbilityModifiers());

	// Draw active cannon UI
	activeCannon->DrawUI();
	activeCannon->DebugInfo();

	// Always the last one to be drawn so that it will always appear infront
	notificationPanel.Draw();
}

void Level::Free() {
	GameTrackingManager::gameData.LevelEnd(levelId);
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

void Level::GetCurrentHP(float& currentHP, bool isGettingPlayerHP) {
	currentHP = 0;
	for (Character const& character : isGettingPlayerHP ? playerCharacters : enemies) {
		currentHP += static_cast<float>(character.attribute.Health());
	}
}

void Level::GetMaxHP(float& maxHP, bool isGettingPlayerHP) {
	maxHP = 0;
	for (Character const& character : isGettingPlayerHP ? playerCharacters : enemies) {
		maxHP += static_cast<float>(character.attribute.MaxHealth());
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
			SwitchTurn(!isPlayerTurn);
		}
	}
}

void Level::SwitchTurn(bool _isPlayerTurn) {
	bool canSetNewRound = true; // always can set new round expect if the current active shooter has an extra shot to shoot
	bool isNewRound = isPlayerStarting == _isPlayerTurn;
	// Check if hasExtraShot ability
	if (hasExtraShot) {
		canSetNewRound = false;
		hasExtraShot = false;
	}
	else {
		isPlayerTurn = _isPlayerTurn; // set isPlayerTurn if there is no extra shot

		// Skip oponennt turn
		if (skipOpponentTurn) {
			isPlayerTurn = !isPlayerTurn; // flip state again 
			skipOpponentTurn = false;
			isNewRound = true;
		}
	}

	// Switching active cannon
	if (isPlayerTurn) {
		activeCannon = &playerCannon;
		// randomise the abilities available to the player for this turn
		AbilitiesManager::playerAbilityController.AbilitySelectionRandomiser();
	}
	else {
		// Enemy Turn
		activeCannon = &enemyCannon;
	}

	std::string newTurnMsg = "Switch to " + std::string(isPlayerTurn ? "PLAYER" : "ENEMY") + " Turn";
	notificationPanel.Popup(newTurnMsg, 2.5f);

	// Turn on/off selecting ability if it player turn or not
	AbilitiesManager::playerAbilityController.IsSelectingAbility(isPlayerTurn);

	// can set new turn and whoever starting the round and current turn is the same
	if (canSetNewRound && isNewRound) {
		UpdateNewRound();
	}

	activeCannon->ResetCannon();
	AudioManager::PlaySFX(AudioManager::SFX::TurnSwitch);
	std::cout << "[Level] Switch to " << std::string(isPlayerTurn ? "PLAYER" : "ENEMY") << " turn" << '\n';
}

void Level::UpdateNewRound() {
	++roundCount;

	// Decaying tiles effect
	if (decayingTiles.size() > 0) {
		for (TileIndex id : decayingTiles) {
			Tile& tile = activeLevelMap[id.y][id.x];
			tile.attribute.UpdateTicks();
			tile.UpdateTextureColor(1);
		}

		// Erase any decaying tiles
		decayingTiles.erase(
			std::remove_if(decayingTiles.begin(), decayingTiles.end(),
				[&](TileIndex id) {
					Tile& tile = activeLevelMap[id.y][id.x];

					if (!tile.attribute.IsTickDamageActive()) {
						tile.UpdateTextureColor(0); // reset texture color back to default
						return true;
					}
					return false;
				}),
			decayingTiles.end()
		);
	}

	// Check for poison characters and update tick
	if (poisonCharacters.size() > 0) {
		for (std::pair<bool, int>& value : poisonCharacters) {
			bool isPlayerCharacter = value.first;
			int characterId = value.second;
			Character* character = isPlayerCharacter ? FindPlayerCharacterObj(characterId) : FindEnemyCharacterObj(characterId);
			character->attribute.UpdateTicks();
			character->UpdateTextureColor(1);
		}

		// Erase any poison characters from the vector if poison is not active anymore
		poisonCharacters.erase(
			std::remove_if(poisonCharacters.begin(), poisonCharacters.end(),
				[&](std::pair<bool, int>const& poisonChar) {
					bool isPlayerCharacter = poisonChar.first;
					int characterId = poisonChar.second;
					Character* character = isPlayerCharacter ? FindPlayerCharacterObj(characterId) : FindEnemyCharacterObj(characterId);

					// Poison is gone
					if (!character->attribute.IsTickDamageActive()) {
						character->UpdateTextureColor(0); // reset texture color back to default
						return true;
					}
					return false;
				}),
			poisonCharacters.end()
		);
	}

	AbilitiesManager::AbilityController* playerController = playerCannon.AbilityController();
	AbilitiesManager::AbilityController* enemyController = enemyCannon.AbilityController();

	// Update turn lifespan of active tower shields
	playerShield.UpdateLifespan();
	enemyShield.UpdateLifespan();

	// Update turn lifespan of all ability modifiers that each controller has
	// and check if its turn lifespan or hp is not zero remove the ability effects
	playerController->UpdateModifiers();
	enemyController->UpdateModifiers();

	RecordPrevDamage(playerDamageLog);
	RecordPrevDamage(enemyDamageLog);
}

void Level::UpdateLevelStatus() {
	status = !AbilitiesManager::playerAbilityController.IsSelectingAbility() && !isHoveringUI && delayTimer <= 0.0f && !prevIsAbilitySelectorOn
		? LevelStatus::PLAYING : LevelStatus::PAUSED;

	// Checking win conditions for both enemy and player
	float currentPlayerHP, currentEnemyHP;
	GetCurrentHP(currentPlayerHP, true);
	GetCurrentHP(currentEnemyHP, false);

	// All the enemies and boss are dead or only the boss are dead player will win
	if (EnemyBoss()) {
		if (EnemyBoss()->attribute.Health() <= 0 || currentEnemyHP <= 0) status = LevelStatus::PLAYER_WON;
	}
	else {
		if (currentEnemyHP <= 0) status = LevelStatus::PLAYER_WON;
	}

	if (currentPlayerHP <= 0) status = LevelStatus::ENEMY_WON;

	// Unlock the next level once, only when victory happens
	if (status == LevelStatus::PLAYER_WON && !hasProcessedVictory) {
		LevelManager::UnlockNextLevel(levelId);
		hasProcessedVictory = true;
	}

	if (status != LevelStatus::PLAYER_WON) {
		hasProcessedVictory = false;
	}
}

void Level::SetTerritoryIndex(TileIndex const& newTileId, TileIndex& center, TileIndex& min, TileIndex& max) {
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
}

void Level::SetOriginalTerritoryTiles(TerritoryTiles& territoryTiles) {
	territoryTiles.originalTiles.reserve(32);
	// Set new min and max tileIndex
	// a box : min = bottom left corner, max = top right corner
	int radius = territoryTiles.territoryTileRadius;
	TileIndex min{ 0,0 }, max{ 0,0 };
	Utils::CalculateTileBounds(territoryTiles.centerId, min, max, radius);

	// Store the starting tiles of the min and max territory to the vector
	for (int y = max.y; y < min.y; ++y) {
		for (int x = min.x; x < max.x; ++x) {
			if (Utils::IsTileIndexOutOfBounds(x, y)) continue; // additional check index not out of bounds
			Tile const& tile = levelMap[y][x];

			if (!tile.IsBlocks()) continue; // not a block continue
			territoryTiles.originalTiles.push_back(tile);
		}
	}
}

void Level::ToggleTerritoryTiles(bool isPlayer, bool isBuilding) {
	TerritoryTiles const& territoryTiles = isPlayer ? playerTerritory : enemyTerritory;
	for (Tile const& originalTile : territoryTiles.originalTiles) {
		Tile& currentTile = activeLevelMap[originalTile.tileId.y][originalTile.tileId.x];
		if (currentTile.IsCharacter()) continue; // Don't rebuild/destroy a tile that contains a character already

		if (isBuilding) { // rebuild
			currentTile.Type(originalTile.Type());
		}
		else { // destroy
			currentTile.Type(TileType::TILE_EMPTY);
		}
	}
}

void Level::RecordDamageLog(int damageToAdd, bool isCharacter) {
	DamageLog& log = isPlayerTurn ? playerDamageLog : enemyDamageLog;
	if (isCharacter) {
		log.roundCharacterDamage += damageToAdd;
		log.totalCharacterDamage += damageToAdd;
	}
	else {
		log.roundTileDamage += damageToAdd;
		log.totalTileDamage += damageToAdd;
	}
}

void Level::RecordPrevDamage(DamageLog& log) {
	log.prevTurnCharacterDamage = log.totalCharacterDamage - log.roundCharacterDamage;
	log.prevTurnTileDamage = log.totalTileDamage - log.roundTileDamage;

	//std::cout << "Damage Log: "
	//	<< "\nTotal Character Damage : " << log.totalCharacterDamage
	//	<< "\nCurrent Round Character Damage: " << log.roundCharacterDamage
	//	<< "\nPrevious Round Character Damage: " << log.prevTurnCharacterDamage
	//	<< "\n\nTotal Tile Damage : " << log.totalTileDamage
	//	<< "\nCurrent Round Tile Damage: " << log.roundTileDamage
	//	<< "\nPrevious Round Tile Damage: " << log.prevTurnTileDamage
	//	<< '\n';

	// Reset current round damage
	log.roundCharacterDamage = 0;
	log.roundTileDamage = 0;
}

void Level::DebugCommands() {
	if (!Utils::isDebuggerOn) return;

	// Input R to change whose turn it is
	if (AEInputCheckTriggered(AEVK_R)) {
		SwitchTurn(!isPlayerTurn);
	}
	// Input CTRL + R to turn autoPlayOn on/off
	if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
		if (AEInputCheckTriggered(AEVK_R)) {
			autoPlayOn = !autoPlayOn;
		}
	}

	// Input E to skip enemy turn
	if (AEInputCheckTriggered(AEVK_E)) {
		noEnemyTurn = !noEnemyTurn;
	}

	// Rebuild tower
	if (AEInputCheckTriggered(AEVK_G)) {
		ToggleTerritoryTiles(true, true);
	}
	// Destroy tower
	if (AEInputCheckTriggered(AEVK_H)) {
		ToggleTerritoryTiles(true, false);
	}

	int gridX = 0, gridY = 0;
	Utils::GetMouseGridIndex(gridX, gridY);

	// Input MOUSE RIGHT CLICK to remove tile blocks or damage characters during in game
	Tile& tile = *(MapManager::FindTile({ gridX, gridY }));
	if (CollisionManager::IsPointInGameObject(tile)) {
		if (AEInputCheckCurr(AEVK_RBUTTON)) {
			if (tile.IsBlocks()) {
				tile.Type(TileType::TILE_EMPTY);
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

	if (noEnemyTurn) { // Skipping enemy turn
		if (!isPlayerTurn) {
			SwitchTurn(true);
		}

		if (autoPlayOn) autoPlayOn = false;
	}

	if (autoPlayOn) {
		AbilitiesManager::playerAbilityController.IsSelectingAbility(false);;
		if (isPlayerTurn) {
			SwitchTurn(false);
		}

		noEnemyTurn = false;
	}
}

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

	std::string debug = "Shortcuts:\n[CTRL + R] Auto play " + std::string(autoPlayOn ? "ON" : "OFF")
		+ "\n[E] Skipping enemy turn " + std::string(noEnemyTurn ? "ON" : "OFF")
		+ "\n[R] Skip to enemy turn\n\nLevel:\nLevel Status: " + std::to_string(static_cast<int>(status))
		+ "\n\nTile Map\nIndex X: " + std::to_string(gridX) + " Y: " + std::to_string(gridY)
		+ "\nTile: " + GetTileName(tile.Type());
	// Display which character is currently at this tile type
	if (tile.IsCharacter()) {
		std::stringstream ss;
		ss << FindCharacterObj(tile);
		debug += " " + ss.str(); // display address of the character object
	}

	debug += "\nPos X: " + Utils::FormatFloat(tile.transform.pos.x, 2) + " Y: " + Utils::FormatFloat(tile.transform.pos.y, 2)
		+ "\nHP: " + Utils::FormatFloat(tile.attribute.Health(), 2)
		+ "\nDamage Multiplier: " + std::to_string(tile.attribute.DamageMultiplier());

	if (tile.attribute.IsTickDamageActive()) {
		debug += tile.attribute.TickDamageInfo();
	}

	debug += "\n\nCamera " + std::to_string(CameraManager::camera.GetState())
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

	debugPanel.Update(debug);
	debugPanel.Draw();

	cannonDebugPanel.Update(activeCannon->DebugInfo());
	cannonDebugPanel.Draw();
}