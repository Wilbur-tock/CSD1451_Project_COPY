#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "pch.hpp"

#include "MapManager.hpp"
#include "CannonManager.hpp"

class Level {
public:
	// Player cannon
	CannonManager::Cannon cannon;
	// Level map
	std::vector<std::vector<MapManager::Tile>> levelMap;

	void Load();
};

#endif // !LEVEL_HPP