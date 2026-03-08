#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "pch.hpp"

#include "CannonManager.hpp"
#include "Tile.hpp"
#include "TextObject.hpp"
#include "Character.hpp"

enum class LevelMode {
	EASY,
	MEDIUM,
	HARD
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
private:
	void DrawUI();

	// Calculate and get the current and max health for all the player characters
	void GetHealth(float& currentHP, float& maxHP, bool isGettingPlayerHP = true);

	// Toggle turns between player and enemy once they have shot and a timer has been set and reduced to 0
	void UpdateTurn();
	void SetTurn(bool _isPlayerTurn);

	void Debugger();

private:
	int levelId = 0;
	std::string levelName; // Name of the level in the format (Level [INDEX])
	std::string levelFileName; // file name to load in the levelMap in the format (level[INDEX])
	LevelMode levelDifficulty = LevelMode::EASY;
	TextObject debugText;
	CannonManager::Cannon *activeCannon = nullptr; // pointer to the active instance of the cannon object 
	CannonManager::Cannon playerCannon; // instance of the player cannon
	CannonManager::Cannon enemyCannon; // instance of the enemy cannon

	bool isPlayerTurn = true; // is player or enemy turn to shoot
	bool autoPlayOn = false; // used for debugging let enemy play ONLY
	bool isSkippingEnemyTurn = false;
	bool isAbilitySelectorOn = true;
	bool isHoveringUI = false;
	float switchTurnCooldown = 0.0f; // Cooldown timer to add a delay between the destruction of the player projectile and starting of enemy turn

	std::vector<std::vector<Tile>> levelMap; // Level tile map
	std::vector<Character> enemies; // contains all instance of the Enemy Character Object in the level
	std::vector<Character> playerCharacters; // contains all instance of the Player Character Object in the level
};

#endif // !LEVEL_HPP