#include "Tile.hpp"

Tile::Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type)
	: GameObject{ _pos, {defaultSize,defaultSize}, _texture, Color{1,1,1} }, type{ _type }, gridId{ 0,0 } {
}

TileType Tile::Type() const { return type; }
void Tile::Type(TileType newType) {
	type = newType;
}

bool Tile::IsGround() const {
	return type >= TILE_BLOCK_GROUND_0 && type <= TILE_BLOCK_GROUND_2;
}
bool Tile::IsBlocks() const {
	return type >= TILE_BLOCK_1_TOP && type <= TILE_BLOCK_2;
}

void Tile::Damage(int damage) {
	attributes.Damage(damage);
	if (attributes.GetHealth() <= 0) {
		type = TILE_EMPTY; //TEMPORARY
	}
	//std::cout << "Damaged Tile x: " << id.x << " y: " << id.y << " health: " << health << std::endl;
}

std::string TileTypeToString(int id) {
	if (id < 0 || id >= static_cast<int>(tileNames.size())) return "INVALID";
	return tileNames[id];
}
