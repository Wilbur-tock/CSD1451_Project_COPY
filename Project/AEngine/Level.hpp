/*
Authors: Dayrius, Nixon
Description: Stores information about each gameplay level (enemies and player units, tiles blocks, etc)
and manage the transition between player and enemy turns. 
Drawing of the game tiles, map and background.

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "pch.hpp"

#include "CannonManager.hpp"
#include "Tile.hpp"
#include "Character.hpp"
#include "TowerShield.hpp"

enum class LevelDifficulty { EASY, MEDIUM, HARD };

enum class LevelStatus {
	PLAYING,
	PAUSED,
	PLAYER_WON,
	ENEMY_WON,
};

struct TerritoryTiles {
	TileIndex centerId{ 0,0 }; // center tile index of where the player/enemy character object is located at
	TileIndex minId{ 0,0 }; // min tile index of where the player/enemy character object is located at
	TileIndex maxId{ 0,0 }; // max tile index of where the player/enemy character object is located at
	std::vector<Tile> originalTiles; // list of tiles objects that contains destructible blocks that belongs to the player/enemy
	int territoryTileRadius = 15;
};

// Track how much damage was deal to the characters and tiles throughout out the whole level playthrough
struct DamageLog {
	int totalTileDamage = 0;
	int totalCharacterDamage = 0;

	int roundTileDamage = 0;
	int roundCharacterDamage = 0;

	int prevTurnTileDamage = 0;
	int prevTurnCharacterDamage = 0;
};

// Class that contains all the level information 
// Level tile map data,
// instances of ALL player and enemy object, 
// instance of ALL player and enemy cannon objects
class Level {
public:
	// Default ctor
	Level(int _leveId);

	// Getters and Setters
public:
	int Id() const { return levelId; }
	std::string const& Name() const { return levelName; }
	int Rounds() const { return roundCount; }

	void Difficulty(int id) { levelDifficulty = static_cast<LevelDifficulty>(id); }
	LevelDifficulty Difficulty() const { return levelDifficulty; }

	TowerShield& GetTowerShield(bool isPlayer) { return isPlayer ? playerShield : enemyShield; }
	CannonManager::Cannon& GetCannon(bool isPlayer) { return isPlayer ? playerCannon : enemyCannon; }
	// Checking who is starting the level
	bool IsStarting(bool isPlayer) const { return isPlayer == isPlayerStarting; }

	std::vector<Character>& GetCharacters(bool isPlayer) { return (isPlayer ? playerCharacters : enemies); }

	// Member functions
public:
	// Read level map data and load level resources
	void Load();
	void Initalize();
	void Update();
	void UpdateUI();
	void Draw();
	void Free();

	// Find the character object that is within the parameter tile, 
	// return pointer to the chracter if a character object can be found otherwise return nullptr
	Character* FindCharacterObj(Tile const& tile);
	Character* FindEnemyCharacterObj(int characterId);
	Character* FindPlayerCharacterObj(int characterId);
	Character* RandomCharacterObj(bool isPlayerObj);

	Character* EnemyBoss() { 
		if (bossId < 0 || bossId >= static_cast<int>(enemies.size())) return nullptr;
		return &enemies[bossId];
	}

	void SetInventoryHoveringUI(bool isHovering) {  isHoveringOverInventoryUI = isPlayerTurn ? isHovering : false; }
	void SetExtraTurn() { hasExtraShot = true; }
	void SetSkipOpponentTurn() { skipOpponentTurn = true; }

	// build or destroy all the original tiles belongs to the territory
	void ToggleTerritoryTiles(bool isPlayer, bool isBuilding);

	void AddDecayTiles(TileIndex const& vectorTileId) { decayingTiles.push_back(vectorTileId); }
	void AddPoisonCharacters(bool isPlayerCharacter, int vectorCharacterId) {  poisonCharacters.push_back({ isPlayerCharacter, vectorCharacterId }); }

	void RecordDamageLog(int damageToAdd, bool isCharacter);

private:
	void InitializeUI();
	void DrawUI();

	// Calculate and get the current health for all the player or enemies characters
	void GetCurrentHP(float& currentHP, bool isGettingPlayerHP = true);
	// Calculate and get the max health for all the player or enemies characters
	void GetMaxHP(float& maxHP, bool isGettingPlayerHP = true);

	// Toggle turns between player and enemy once they have shot and a timer has been set and reduced to 0
	void UpdateTurn();
	void SwitchTurn(bool _isPlayerTurn);
	void UpdateNewRound();

	void UpdateLevelStatus();

	void SetTerritoryIndex(TileIndex const& newTileId, TileIndex& center, TileIndex& min, TileIndex& max);
	void SetOriginalTerritoryTiles(TerritoryTiles& territoryTiles);

	void RecordPrevDamage(DamageLog& log);

	void DebugCommands();
	void DebugInfo();

private:
	int levelId = 0;
	std::string levelName; // Name of the level in the format (Level [INDEX])
	std::string levelFileName; // file name to load in the levelMap in the format (level[INDEX])
	LevelDifficulty levelDifficulty = LevelDifficulty::EASY;

	LevelStatus status = LevelStatus::PLAYING;
	int roundCount = 0; // Number of rounds after both player/enemy has taken turn to shoot

	CannonManager::Cannon* activeCannon = nullptr; // pointer to the active instance of the cannon object 
	CannonManager::Cannon playerCannon; // instance of the player cannon
	CannonManager::Cannon enemyCannon; // instance of the enemy cannon

	bool isPlayerStarting = true; // is player the first one to shoot
	bool isPlayerTurn = true; // is player or enemy turn to shoot
	bool autoPlayOn = false; // used for debugging let enemy play ONLY
	bool hasExtraShot = false; // does the current shooter have an extra shot
	bool skipOpponentTurn = false; // skip enemy turn to shot

	bool disableEnemyTurn = false; // used for debugger to toggle on/off of skipping enemy round
	bool noEnemy = false; // if true no enemy can shoot at all can't toggle enemy on and off even with debugger on
	bool canSelectAbility = true;

	bool resetMapEveryRound = false;
	bool isHoveringUI = false;
	bool isHoveringOverInventoryUI = false;
	float switchTurnCooldown = 0.0f; // Cooldown timer to add a delay between the destruction of the player projectile and starting of enemy turn

	// Map level data
	std::vector<std::vector<Tile>> levelMap; // Default Level tile map loaded at the start
	std::vector<std::vector<Tile>> activeLevelMap; // runtime level map

	// Level characters object
	int bossId = 0; // enemies vector id which the boss is stored in
	std::vector<Character> enemies; // contains all instance of the Enemy Character Object in the level
	std::vector<Character> playerCharacters; // contains all instance of the Player Character Object in the level

	// Territory tiles
	int groundY = 0;
	TerritoryTiles playerTerritory, enemyTerritory;

	TowerShield playerShield, enemyShield;

	std::vector<TileIndex> decayingTiles; // vector ids of activeLevelMap of which tiles having the decaying effect
	std::vector<std::pair<bool,int>> poisonCharacters; // vector ids of all the characters that are poison, bool isPlayerCharacter, int vectorId

	DamageLog playerDamageLog, enemyDamageLog;
	
	bool hasProcessedVictory = false;

	// Sky base texture drawn behind the tile map each frame.
	// PLACEHOLDER: provide Assets/level_sky.png (1600x900 static sky image).
	// Clouds and projectiles are handled by BgAnimation and need no member state here.
	AEGfxTexture* bgTexture = nullptr;
};

#endif // !LEVEL_HPP