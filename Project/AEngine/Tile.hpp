#ifndef TILE_HPP
#define TILE_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Attributes.hpp"

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
	float const defaultSize = Config::TILE_SIZE;
	TileType prevType = TILE_EMPTY; // previous tile type
	TileType type = TILE_EMPTY; // current tile type
public:
	AEVec2 gridId{ 0,0 }; // Grid index x and y
	Attributes attributes;

	Tile() = default;
	Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type);

	TileType Type() const;
	void Type(TileType newType);

	bool IsGround() const;
	bool IsBlocks() const;

	void Damage(int damage);
};


std::string TileTypeToString(int id);


#endif // !TILE_HPP
