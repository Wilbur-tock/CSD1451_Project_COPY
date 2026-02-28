#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "pch.hpp"

#include "CannonManager.hpp"
#include "Tile.hpp"
#include "TextObject.hpp"
#include "Character.hpp"

class Level {
public:
	TextObject debugText;

	Level(std::string levelName);

	// Read level map data and load level resources
	void Load();
	void Initalize();
	void Update();
	void Draw();
	void Free();

	// Member functions

	// Toggle turns between player and enemy once they have shot and a timer has been set and reduced to 0
	void ToggleTurn();
	void SwitchCannon();
	std::string DebugInfo();

	Character* FindEnemyCharacterObj(int characterId);
	Character* FindPlayerCharacterObj(int characterId);
private:
	std::string levelName;
	CannonManager::Cannon *activeCannon = nullptr; // pointer to the active instance of the cannon object 
	CannonManager::Cannon playerCannon; // instance of the player cannon
	CannonManager::Cannon enemyCannon; // instance of the enemy cannon

	bool isPlayerTurn = true; // is player or enemy turn to shoot
	bool autoPlayOn = false; // used for debugging let enemy play ONLY
	float turnTimer = 0.0f;

	std::vector<std::vector<Tile>> levelMap; // Level map
	std::vector<Character> enemies; // contains all instance of the Enemy Character Object in the level
	std::vector<Character> playerCharacters; // contains all instance of the Player Character Object in the level
};

#endif // !LEVEL_HPP