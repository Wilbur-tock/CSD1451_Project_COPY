#ifndef MAPMANAGER_HPP
#define MAPMANAGER_HPP

#include "pch.hpp"

namespace MapManager {
	enum TileType {
		Tile_Air,
		Tile_Player,
		Tile_Cannon,
		Tile_Enemy_Minion,
		Tile_Enemy_Boss,
		Tile_Ground,
		Tile_Block_1,
		Tile_Block_2,
		Tile_Count, // Count of the enum always at the last index
	};

	class Tile {
		int size = Config::TILE_SIZE;
	public:
		TileType type = Tile_Air;
		AEVec2 pos{ 0.0f,0.0f };
	};

	extern std::vector<std::vector<Tile>> mapLayout;

	// Read map data from txt file and then update the data to 2D vector whatever or not barrier is needed
	void LoadMapData(const char* fileName, std::vector<std::vector<Tile>>& map);

	// Rendering of the map layout
	void RenderMap(const std::vector<std::vector<Tile>>& map);
}

#endif // !MapManager_HPP
