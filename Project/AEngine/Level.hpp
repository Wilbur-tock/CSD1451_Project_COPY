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

	std::string DebugInfo();
	Enemy* FindEnemy(int enemyId);
private:
	std::string levelName;
	CannonManager::Cannon playerCannon;
	AEVec2 playerCannonStartPos{ 0,0 };

	std::vector<std::vector<Tile>> levelMap; // Level map
	std::vector<Enemy> enemies;

	// min and max position that has ground collision
	AEVec2 minGround{ 0,0 }, maxGround{ 0,0 };
};

#endif // !LEVEL_HPP