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

	// Checks for level
	int Id() const { return levelId; }
	std::string const& Name() const { return levelName; }
	// Getters and Setters

	TowerShield& GetTowerShield(bool isPlayer) { return isPlayer ? playerShield : enemyShield; }
	CannonManager::Cannon& GetCannon(bool isPlayer) { return isPlayer ? playerCannon : enemyCannon; }
	// Checking who is starting the level
	bool IsStarting(bool isPlayer) const { return isPlayer == isPlayerStarting; }

	std::vector<Character>& GetCharacters(bool isPlayer) { return (isPlayer ? playerCharacters : enemies); }

	// Member functions

	// Read level map data and load level resources
	void Load();
	void Initalize();
	void Update();
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

	void SetHoveringOverUI(bool isHovering) { isHoveringUI = isHovering; }
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
	void SwitchCannon(bool _isPlayerTurn);
	void UpdateNewRound();

	void UpdateLevelStatus();
	void DrawResultsUI();

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
	int roundCount = 1; // Number of turn after both player has shot

	CannonManager::Cannon* activeCannon = nullptr; // pointer to the active instance of the cannon object 
	CannonManager::Cannon playerCannon; // instance of the player cannon
	CannonManager::Cannon enemyCannon; // instance of the enemy cannon

	bool isPlayerStarting = true; // is player the first one to shoot
	bool isPlayerTurn = true; // is player or enemy turn to shoot
	bool autoPlayOn = false; // used for debugging let enemy play ONLY
	bool hasExtraShot = false; // does the current shooter have an extra shot
	bool skipOpponentTurn = false;
	bool noEnemyTurn = false; // used for debugging to keep skipping enemy turns
	bool isHoveringUI = false;
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
};

#endif // !LEVEL_HPP