#ifndef MAPMANAGER_HPP
#define MAPMANAGER_HPP

#include "pch.hpp"

namespace MapManager {
	enum TileType {
		Tile_Air,
		Tile_Barrier,
		Tile_Block,
		Tile_Cannon,
		Tile_Enemy_Minion,
		Tile_Enemy_Boss,
	};

	class Tile {
	public:
		TileType type = Tile_Air;
		int size = 32;
	};

	extern std::vector<std::vector<Tile>> mapLayout;

	/* Load map data and update the reference to a 2D vector */
	void LoadMapData(const char* fileName, std::vector<std::vector<Tile>>& map);

	/* Rendering of the map layout */
	void RenderMap(const std::vector<std::vector<Tile>>& map);
}

#endif // !MapManager_HPP
