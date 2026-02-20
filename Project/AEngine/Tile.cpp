#include "Tile.hpp"

Tile::Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type)
	: GameObject{ _pos, {defaultSize,defaultSize}, _texture }, type{ _type } {}

TileType Tile::Type() const { return type; }
void Tile::Type(TileType newType) {
	prevType = type;
	type = newType;
	if (IsEmpty() && IsEnemy()) enemyId = -1;
}

void Tile::Type(Tile const& toCopy) {
	Type(toCopy.type);

	enemyId = toCopy.EnemyId();
	attribute = toCopy.attribute;
}

int  Tile::EnemyId() const { return enemyId; }
void Tile::EnemyId(int newId) { enemyId = newId; }

bool Tile::IsEmpty() const { return type == TILE_EMPTY; }
bool Tile::IsGround() const {
	return type >= TILE_BLOCK_GROUND_0 && type <= TILE_BLOCK_GROUND_2;
}
bool Tile::IsBlocks() const {
	return type >= TILE_BLOCK_1_TOP && type <= TILE_BLOCK_2;
}

bool Tile::IsEnemy() const { return type == TILE_ENEMY_MINION; }

void Tile::TakeDamage(int damage) {
	attribute.TakeDamage(damage * attribute.DamageMultiplier());

	if (!attribute.IsActive()) {
		Type(TILE_EMPTY);
	}
}

std::ostream& operator<<(std::ostream& os, Tile const& rhs) {
	os << TileTypeToString(rhs.Type()) << " " << rhs.tileId;
	return os;
}

std::string TileTypeToString(int id) {
	if (id < 0 || id >= static_cast<int>(tileNames.size())) return "INVALID";
	return tileNames[id];
}
