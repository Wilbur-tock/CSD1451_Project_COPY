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

	// Blocks that can be destroy
	TILE_BLOCK_1_TOP, // Always the first destructible block
	TILE_BLOCK_1,
	TILE_BLOCK_2_TOP,
	TILE_BLOCK_2, // Always the last destructible block

	TILE_PLAYER_CANNON,
	TILE_PLAYER_CHARACTER,

	TILE_ENEMY_CANNON,
	TILE_ENEMY_MINION,
	TILE_ENEMY_BOSS,

	TILE_COUNT, // Count of the enum always at the last index
};

inline std::vector<std::string> tileNames = { 
	"Air", "Grass", "Dirt", "Dirt with Stones", "Block_1_TOP", "Block_1", "Block_2_TOP", "Block_2",
	"Player Cannon", "Player Character",
	"Enemy Cannon", "Enemy Minion", "Enemy Boss" 
};

class Tile : public GameObject {
public:
	Attributes attribute;

	Tile() = default;
	Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type);

	TileType Type() const;
	void Type(TileType newType);

	// Copy another type data
	void Type(Tile const& toCopy);

	int CharacterId() const;
	void CharacterId(int newId);

	bool IsEmpty() const;
	bool IsGround() const;
	bool IsBlocks() const;

	bool IsEnemy() const;
	bool IsPlayer() const;
	bool ContainCharacter() const;

	void TakeDamage(int damage);
private:
	float const defaultSize = Config::TILE_SIZE;
	TileType prevType = TileType::TILE_EMPTY; // previous tile type
	TileType type = TileType::TILE_EMPTY; // current tile type
	int enemyId = -1; // index of the enemy object stored in the vector list
};

std::ostream& operator<<(std::ostream& os, Tile const& rhs);
std::string GetTileName(int id);

#endif // !TILE_HPP
