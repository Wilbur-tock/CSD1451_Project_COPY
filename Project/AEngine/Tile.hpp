#ifndef TILE_HPP
#define TILE_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Attributes.hpp"

enum class TileType {
	TILE_EMPTY,

	TILE_BLOCK_GROUND_0, // Must the first ground index
	TILE_BLOCK_GROUND_1,
	TILE_BLOCK_GROUND_2,
	TILE_BLOCK_CANNON_BASE_TOP,
	TILE_BLOCK_CANNON_BASE,

	// Blocks that can be destroy
	TILE_BLOCK_1_TOP, // Always the first destructible block
	TILE_BLOCK_1,
	TILE_BLOCK_2_TOP,
	TILE_BLOCK_2, // Always the last destructible block

	TILE_PLAYER_CANNON		= 10,
	TILE_PLAYER_CHARACTER	= 11,

	TILE_ENEMY_CANNON		= 12,
	TILE_ENEMY_MINION		= 13,
	TILE_ENEMY_BOSS			= 14,

	TILE_COUNT, // Count of the enum always at the last index
};

inline std::vector<std::string> tileNames = { 
	"Air", "Grass", "Dirt", "Dirt with Stones", "Cannon Base Top", "Cannon Base",
	"Block_1_TOP", "Block_1", "Block_2_TOP", "Block_2",
	"Player Cannon", "Player Character",
	"Enemy Cannon", "Enemy Minion", "Enemy Boss" 
};

class Tile : public GameObject {
public:
	Attributes attribute;

	Tile() = default;
	Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type);

	TileType Type() const { return type; }

	void Type(TileType newType);
	// Copy another type data
	void Type(Tile const& toCopy);

	int  CharacterId() const { return characterId; }
	void CharacterId(int newId) { characterId = newId; }

	bool IsEmpty() const { return type == TileType::TILE_EMPTY; }
	bool IsGround() const { return type >= TileType::TILE_BLOCK_GROUND_0 && type <= TileType::TILE_BLOCK_GROUND_2;}
	bool IsBlocks() const { return type >= TileType::TILE_BLOCK_1_TOP && type <= TileType::TILE_BLOCK_2;}

	bool IsEnemy() const { return type == TileType::TILE_ENEMY_MINION || type == TileType::TILE_ENEMY_BOSS; }
	bool IsPlayer() const { return type == TileType::TILE_PLAYER_CHARACTER; }
	bool IsCharacter() const { return IsEnemy() || IsPlayer(); }

	bool IsDestructible() const { return IsEnemy() || IsPlayer() || IsBlocks(); }

	// Damage tile and character inside the tile
	void TakeDamage(int damage);
private:
	float defaultSize = Config::TILE_SIZE;
	TileType prevType = TileType::TILE_EMPTY; // previous tile type
	TileType type = TileType::TILE_EMPTY; // current tile type
	int characterId = -1; // index of the character object stored in the vector list
};

std::string GetTileName(TileType type);

std::ostream& operator<<(std::ostream& os, Tile const& rhs);


#endif // !TILE_HPP
