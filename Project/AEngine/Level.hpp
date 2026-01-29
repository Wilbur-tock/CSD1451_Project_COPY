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

	// Read level map data and load level resources
	void Load(std::string filePath);
};

#endif // !LEVEL_HPP