#include "Character.hpp"

#include "UIManager.hpp"
#include "GraphicsManager.hpp"

Character::Character(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId, CharacterType _type)
	: GameObject{ _pos, _size, _texture, _tileId, Color{0.5f,1,0.5f} }, type{_type}{
	attribute.MaxHealth(100);
	if (type != CharacterType::PLAYER) textureColor = { 0.75f,0,0 };
}

CharacterType Character::Type() const { return type; }

void Character::Update() {
	physics.Update(*this);
	Render();
}

void Character::Render() {
	if (!attribute.IsActive()) return;

	GraphicsManager::DrawGameObject(*this);
	// UI
	float x = transform.pos.x, y = transform.pos.y + transform.size.y;
	UIManager::Bar bar{ { x, y}, { transform.size.x, 10 }, Color{1,0,0} };
	bar.Draw(static_cast<float>(attribute.Health()), static_cast<float>(attribute.MaxHealth()));
}

void Character::DebugInfo(std::string& str) {
	std::stringstream ss;
	ss << &*this; // Display address of the character object
	str = "\n\nCharacter Info: " + ss.str()
		+ "\nType: " + TypeName()
		+ "\nCharacter Id: " + std::to_string(id)
		+ "\nTile Id x: " + std::to_string(tileId.x) + " y : " + std::to_string(tileId.y)
		+ "\nPos x: " + Utils::FormatFloat(transform.pos.x, 2) + " y: " + Utils::FormatFloat(transform.pos.y, 2)
		+ "\nSize x: " + Utils::FormatFloat(transform.size.x, 2) + " y: " + Utils::FormatFloat(transform.size.x, 2)
		+ "\nIsGrounded: " + static_cast<std::string>(physics.IsGrounded() ? "true" : "false")
		+ "\n\nStats:\nHP: " + std::to_string(attribute.Health()) + " Max HP: " + std::to_string(attribute.MaxHealth())
		+ "\nDamage Multiplier: " + std::to_string(attribute.DamageMultiplier());
}