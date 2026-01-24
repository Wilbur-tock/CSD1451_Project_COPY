#ifndef MAPMANAGER_HPP
#define MAPMANAGER_HPP

#include "pch.hpp"

namespace MapManager {
	enum TileType {
		Tile_Air,
		Tile_Barrier,
		Tile_Block,
	};

	class Tile {
	public:
		TileType type = Tile_Air;
		int size = 32;
	};

	extern Tile mapLayout[Config::MAP_HEIGHT][Config::MAP_WIDTH];

	void LoadAllMapData();

	/* Rendering of the map layout */
	void RenderMap(AEGfxTexture* tileTexture, Tile const mapLayout[Config::MAP_HEIGHT][Config::MAP_WIDTH]);
}

#endif // !MapManager_HPP
