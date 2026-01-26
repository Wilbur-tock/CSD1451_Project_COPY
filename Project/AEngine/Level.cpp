#include "pch.hpp"
#include "Level.hpp"

void Level::Load() {
	levelMap.resize(Config::GRID_HEIGHT, std::vector<MapManager::Tile>(Config::GRID_WIDTH));

	AEVec2 v2{ 0,0 };
	cannon = CannonManager::Cannon(v2, 10);
}