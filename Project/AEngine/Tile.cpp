#include "Tile.hpp"

#include "LevelManager.hpp"

Tile::Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type)
	: GameObject{ _pos, {defaultSize,defaultSize}, _texture }, type{ _type } {
}

Tile::Tile(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileType _type)
	: GameObject{ _pos, _size, _texture }, type{ _type } {
}

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

void Tile::TakeDamage(int damage) {
	if (!IsDestructible()) return;

	// Damage character here
	if (IsCharacter()) {
		Character* character = LevelManager::CurrentLevel()->FindCharacterObj(*this);
		if (character) {
			character->attribute.TakeDamage(damage);
			std::cout << "[Tile] Damaged character " + character->TypeName() + " new hp: " + std::to_string(character->attribute.Health()) + '\n';
			attribute.Health(character->attribute.Health()); // Assign tile health to character health
		}
	}
	else {
		attribute.TakeDamage(damage * attribute.DamageMultiplier());
	}

	// Blocks can be destroyed -> become empty
	if (!attribute.IsActive()) {
		Type(TileType::TILE_EMPTY);
	}
}

std::string GetTileName(TileType type) {
	int id = static_cast<int>(type);
	if (id < 0 || id >= static_cast<int>(tileNames.size())) return "INVALID";
	return tileNames[id];
}

std::ostream& operator<<(std::ostream& os, Tile const& rhs) {
	os << GetTileName(rhs.Type()) << " " << rhs.tileId;
	return os;
}

