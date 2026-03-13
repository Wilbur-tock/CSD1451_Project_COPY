#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "pch.hpp"

#include "CannonManager.hpp"
#include "Tile.hpp"
#include "Character.hpp"

enum class LevelMode {
	EASY,
	MEDIUM,
	HARD
};

enum class LevelStatus {
	PLAYING,
	PAUSED,
	PLAYER_WON,
	ENEMY_WON,
};

// Class that contains all the level information 
// Level tile map data,
// instances of ALL player and enemy object, 
// instance of ALL player and enemy cannon objects
class Level {
public:
	// Default ctor
	Level(int _leveId);

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
		if (bossId < 0 || bossId > enemies.size()) return nullptr;
		return &enemies[bossId];
	}

	void SetHoveringOverUI(bool isHovering) { isHoveringUI = isHovering; }
private:
	void InitializeUI();
	void DrawUI();

	// Calculate and get the current health for all the player or enemies characters
	void GetCurrentHP(float& currentHP, bool isGettingPlayerHP = true);
	// Calculate and get the max health for all the player or enemies characters
	void GetMaxHP(float& maxHP, bool isGettingPlayerHP = true);

	// Toggle turns between player and enemy once they have shot and a timer has been set and reduced to 0
	void UpdateTurn();
	void SetTurn(bool _isPlayerTurn);
	void UpdateLevelStatus();
	void DrawResultsUI();

	void DebugCommands();
	void DebugInfo();

private:
	int levelId = 0;
	std::string levelName; // Name of the level in the format (Level [INDEX])
	std::string levelFileName; // file name to load in the levelMap in the format (level[INDEX])
	LevelMode levelDifficulty = LevelMode::EASY;

	LevelStatus status = LevelStatus::PLAYING;
	int turnCount = 1; // Number of turn after both player has shot

	CannonManager::Cannon* activeCannon = nullptr; // pointer to the active instance of the cannon object 
	CannonManager::Cannon playerCannon; // instance of the player cannon
	CannonManager::Cannon enemyCannon; // instance of the enemy cannon

	bool isPlayerTurn = true; // is player or enemy turn to shoot
	bool autoPlayOn = false; // used for debugging let enemy play ONLY
	bool isSkippingEnemyTurn = false;
	bool isHoveringUI = false;
	float switchTurnCooldown = 0.0f; // Cooldown timer to add a delay between the destruction of the player projectile and starting of enemy turn

	std::vector<std::vector<Tile>> levelMap; // Default Level tile map loaded at the start
	std::vector<std::vector<Tile>> activeLevelMap; // runtime level map

	std::vector<Character> enemies; // contains all instance of the Enemy Character Object in the level
	int bossId = 0; // enemies vector id which the boss is stored in
	std::vector<Character> playerCharacters; // contains all instance of the Player Character Object in the level
};

#endif // !LEVEL_HPP