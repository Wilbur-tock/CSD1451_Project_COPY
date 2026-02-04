#ifndef MAPMANAGER_HPP
#define MAPMANAGER_HPP

#include "pch.hpp"
#include "GameObject.hpp"

// Contain enum TileType, class Tile
namespace MapManager {
	enum TileType {
		TILE_EMPTY,
		TILE_BLOCK_GROUND_0, // Must the first ground index
		TILE_BLOCK_GROUND_1,
		TILE_BLOCK_GROUND_2,
		TILE_BLOCK_1_TOP,
		TILE_BLOCK_1, //Block Body
		TILE_BLOCK_2_TOP,
		TILE_BLOCK_2, // Block Body
		TILE_PLAYER_CANNON,
		TILE_PLAYER_CHARACTER,
		TILE_ENEMY_CANNON,
		TILE_ENEMY_MINION,
		TILE_ENEMY_BOSS,
		TILE_COUNT, // Count of the enum always at the last index
	};

	inline std::vector<std::string> tileNames = { "Air", "Grass", "Dirt", "Dirt with Stones", "Block_1_TOP", "Block_1", "Block_2_TOP", "Block_2",
		"Player Cannon", "Player Character", 
		"Enemy Cannon", "Enemy Minion", "Enemy Boss" };

	class Tile : public GameObject {
		float defaultSize = Config::TILE_SIZE;
		float maxHealth{ 100.0f }, minHealth{ 0.0f }, health{ 100.0f };
	public:
		TileType type = TILE_EMPTY;
		AEVec2 gridId{0,0}; // Grid index x and y

		Tile() = default;
		Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type);

		void Damage(float damage);

		float GetHealth() const;
		bool IsGround() const;
		bool IsBlocks() const;
	};

	extern std::vector<std::vector<Tile>> *currentMap; // Current map layout that is being displayed onto the map

	std::string TileTypeToString(int id);

	void Load();
	// Read map data from txt file and then update the data to 2D vector whatever or not barrier is needed
	void ReadMapData(std::string fileName, std::vector<std::vector<Tile>>& map);
	// Rendering of the map layout
	void RenderMap(const std::vector<std::vector<Tile>>& map);
}

#endif // !MapManager_HPP
