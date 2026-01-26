#ifndef MAPMANAGER_HPP
#define MAPMANAGER_HPP

#include "pch.hpp"

namespace MapManager {
	enum TileType {
		Tile_Air,
		Tile_Block,
	};

	class Tile {
	public:
		TileType type = Tile_Air;
		int size = 32;
	};

	extern Tile mapLayout[Config::mapHeight][Config::mapWidth];

	void LoadAllMapData();

	/* Rendering of the map layout */
	void RenderMap(AEGfxTexture* tileTexture, Tile const mapLayout[Config::mapHeight][Config::mapWidth]);
}

#endif // !MapManager_HPP
