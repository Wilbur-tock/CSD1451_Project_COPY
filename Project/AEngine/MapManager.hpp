#ifndef MAPMANAGER_HPP
#define MAPMANAGER_HPP

#include "pch.hpp"

#include "Tile.hpp"

// Contain enum TileType, class Tile
namespace MapManager {

	extern std::vector<std::vector<Tile>> *currentMap; // Current map layout that is being displayed onto the map

	void Load();
	// Read map data from txt file and then update the data to 2D vector whatever or not barrier is needed
	void ReadMapData(std::string fileName, std::vector<std::vector<Tile>>& map);
	// Rendering of the map layout
	void RenderMap(const std::vector<std::vector<Tile>>& map);
	Tile* FindTile(TileIndex const& tileId);
}

#endif // !MapManager_HPP
