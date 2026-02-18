#include "Tile.hpp"

Tile::Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type)
	: GameObject{ _pos, {defaultSize,defaultSize}, _texture, Color{1,1,1} }, gridId{ 0,0 }, type{ _type } {}

TileType Tile::Type() const { return type; }
void Tile::Type(TileType newType) {
	prevType = type;
	type = newType;
}

bool Tile::IsGround() const {
	return type >= TILE_BLOCK_GROUND_0 && type <= TILE_BLOCK_GROUND_2;
}
bool Tile::IsBlocks() const {
	return type >= TILE_BLOCK_1_TOP && type <= TILE_BLOCK_2;
}

bool Tile::IsEnemy() const { return type == TILE_ENEMY_MINION; }

void Tile::TakeDamage(int damage) {
	attribute.TakeDamage(damage * attribute.DamageModifier());
	if (attribute.Health() <= 0) {
		Type(TILE_EMPTY);
	}
	//std::cout << "Damaged Tile x: " << id.x << " y: " << id.y << " health: " << health << std::endl;
}

std::string TileTypeToString(int id) {
	if (id < 0 || id >= static_cast<int>(tileNames.size())) return "INVALID";
	return tileNames[id];
}
