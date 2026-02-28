#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "pch.hpp"

#include "CannonManager.hpp"
#include "Tile.hpp"
#include "TextObject.hpp"
#include "Enemy.hpp"

class Level {
public:
	TextObject debugText;

	// Read level map data and load level resources
	void Load(std::string const& filePath);
	void Initalize();
	void Update();
	void Free();

	// Member functions

	// Toggle turns between player and enemy once they have shot and a timer has been set and reduced to 0
	void ToggleTurn();
	void SwitchCannon();
	std::string DebugInfo();
	Enemy* FindEnemy(int enemyId);
private:
	std::string levelName;
	CannonManager::Cannon *activeCannon; // pointer to the active instance of the cannon object 
	CannonManager::Cannon playerCannon; // instance of the player cannon
	CannonManager::Cannon enemyCannon; // instance of the enemy cannon

	bool isPlayerTurn = true; // is player or enemy turn to shoot
	float turnTimer = 0.0f;

	std::vector<std::vector<Tile>> levelMap; // Level map
	std::vector<Enemy> enemies; // contains all instance of the Enemy Object in the level
};

#endif // !LEVEL_HPP