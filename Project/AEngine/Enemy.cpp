#include "Enemy.hpp"

#include "UIManager.hpp"
#include "GraphicsManager.hpp"

Enemy::Enemy(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId)
	: GameObject{ _pos, _size, _texture, _tileId, Color{0.5f,1,0.5f} }, type{EnemyType::MINION}{
	attribute.MaxHealth(100);
}

EnemyType Enemy::Type() const { return type; }

void Enemy::Update() {
	physics.Update(*this);

	Render();
}

void Enemy::Render() {
	if (!attribute.IsActive()) return;

	GraphicsManager::DrawGameObject(*this);
	// UI
	UIManager::Bar bar{ { transform.pos.x, transform.pos.y + transform.size.y }, { transform.size.x, 10 }, Color{1,0,0} };
	bar.Draw(static_cast<float>(attribute.Health()), static_cast<float>(attribute.MaxHealth()));
}

void Enemy::DebugInfo(std::string& str) {
	std::stringstream ss;
	ss << &*this;
	str = "\n\nEnemy Info: " + ss.str()
		+ "\nId: x:" + std::to_string(tileId.x) + " y : " + std::to_string(tileId.y)
		+ "\nPos x : " + Utils::FormatFloat(transform.pos.x, 2) + " y: " + Utils::FormatFloat(transform.pos.y, 2)
		+ "\nSize x: " + Utils::FormatFloat(transform.size.x, 2) + " y: " + Utils::FormatFloat(transform.size.x, 2)
		+ "\nHP: " + std::to_string(attribute.Health()) + " Max HP: " + std::to_string(attribute.MaxHealth())
		+ "\nDamage Multiplier: " + std::to_string(attribute.DamageMultiplier())
		+ "\nIsGrounded: " + std::string(physics.IsGrounded() ? "true" : "false");
}