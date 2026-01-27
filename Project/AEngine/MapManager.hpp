#ifndef MAPMANAGER_HPP
#define MAPMANAGER_HPP

#include "pch.hpp"
#include "GameObject.hpp"

// Contain enum TileType, class Tile
namespace MapManager {
	enum TileType {
		TILE_EMPTY,
		TILE_BLOCK_GROUND,
		TILE_BLOCK_1,
		TILE_BLOCK_2,
		TILE_PLAYER_CANNON,
		TILE_PLAYER_CHARACTER,
		TILE_ENEMY_MINION,
		TILE_ENEMY_BOSS,
		TILE_COUNT, // Count of the enum always at the last index
	};

	class Tile : public GameObject {
		float size = Config::TILE_SIZE;
	public:
		TileType type = TILE_EMPTY;

		Tile() = default;
		Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type);
	};

	extern std::vector<std::vector<Tile>> mapLayout;

	// Read map data from txt file and then update the data to 2D vector whatever or not barrier is needed
	void ReadMapData(std::string fileName, std::vector<std::vector<Tile>>& map);

	// Rendering of the map layout
	void RenderMap(const std::vector<std::vector<Tile>>& map);
}

#endif // !MapManager_HPP
