#include "Enemy.hpp"

#include "UIManager.hpp"
#include "GraphicsManager.hpp"

Enemy::Enemy(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId)
	: GameObject{ _pos, _size, _texture, _tileId, Color{0.5f,1,0.5f} }, type{MINION}{
}

EnemyType Enemy::Type() const { return type; }

void Enemy::Update() {
	physics.Update(*this, tileId);

	Render();
}

void Enemy::Render() {
	GraphicsManager::DrawGameObject(*this);
	// UI
	UIManager::Bar bar{ { pos.x, pos.y + size.y }, { size.x, 10 }, Color{1,0,0} };
	bar.Draw(static_cast<float>(attribute.Health()), static_cast<float>(attribute.MaxHealth()));
}

void Enemy::DebugInfo(std::string& str) {
	std::stringstream ss;
	ss << &*this;
	str = "\n\nEnemy Info: " + ss.str()
		+ "\nId: x:" + std::to_string(tileId.x) + " y : " + std::to_string(tileId.y)
		+ "\nPos x : " + Utils::FormatFloat(pos.x, 2) + " y: " + Utils::FormatFloat(pos.y, 2)
		+ "\nSize x: " + Utils::FormatFloat(size.x, 2) + " y: " + Utils::FormatFloat(size.x, 2)
		+ "\nHP: " + std::to_string(attribute.Health())
		+ "\nDamage Multiplier: " + std::to_string(attribute.DamageMultiplier())
		+ "\nIsGrounded: " + std::string(physics.IsGrounded() ? "true" : "false");
}