#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "pch.hpp"

#include "MapManager.hpp"
#include "CannonManager.hpp"

class Level {
public:
	// Player cannon
	CannonManager::Cannon cannon;
	AEVec2 playerCannonStartPos{ 0,0 };
	// Level map
	std::vector<std::vector<MapManager::Tile>> levelMap;

	// min and max position that has ground collision
	AEVec2 minGround{ 0,0 }, maxGround{ 0,0 };

	TextObject debugText;

	// Read level map data and load level resources
	void Load(std::string filePath);
	void Init();
};

#endif // !LEVEL_HPP