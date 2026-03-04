#include "Tile.hpp"

Tile::Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type)
	: GameObject{ _pos, {defaultSize,defaultSize}, _texture }, type{ _type } {
}

TileType Tile::Type() const { return type; }
void Tile::Type(TileType newType) {
	prevType = type;
	if (IsEmpty() && IsCharacter()) characterId = -1; // reset characterId
	type = newType;
}

void Tile::Type(Tile const& toCopy) {
	Type(toCopy.type);

	characterId = toCopy.CharacterId();
	attribute = toCopy.attribute;
}

int  Tile::CharacterId() const { return characterId; }
void Tile::CharacterId(int newId) { characterId = newId; }

bool Tile::IsEmpty() const { return type == TILE_EMPTY; }
bool Tile::IsGround() const {
	return type >= TILE_BLOCK_GROUND_0 && type <= TILE_BLOCK_GROUND_2;
}
bool Tile::IsBlocks() const {
	return type >= TILE_BLOCK_1_TOP && type <= TILE_BLOCK_2;
}

bool Tile::IsEnemy() const { return type == TILE_ENEMY_MINION || type == TILE_ENEMY_BOSS; }
bool Tile::IsPlayer() const { return type == TILE_PLAYER_CHARACTER; }
bool Tile::IsCharacter() const { return IsEnemy() || IsPlayer(); }

void Tile::TakeDamage(int damage) {
	// Only tower blocks take damage. Ground stays untouched.
	if (!IsBlocks()) return;

	attribute.TakeDamage(damage * attribute.DamageMultiplier());

	// Blocks can be destroyed -> become empty
	if (!attribute.IsActive()) {
		Type(TileType::TILE_EMPTY);
	}
}

std::ostream& operator<<(std::ostream& os, Tile const& rhs) {
	os << GetTileName(rhs.Type()) << " " << rhs.tileId;
	return os;
}

std::string GetTileName(int id) {
	if (id < 0 || id >= static_cast<int>(tileNames.size())) return "INVALID";
	return tileNames[id];
}
