#include "Enemy.hpp"

#include "UIManager.hpp"
#include "GraphicsManager.hpp"

Enemy::Enemy(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, std::pair<int, int> _tileId)
	: GameObject{ _pos, _size, _texture, Color{0.5f,1,0.5f} }, type{MINION}, tileId{_tileId} {
}

EnemyType Enemy::Type() const { return type; }
std::pair<int,int> Enemy::TileId() const { return tileId; }
void Enemy::TileId(std::pair<int, int> newId) { 
	tileId.first  = newId.first; 
	tileId.second = newId.second;
}

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
	str = "\n\nEnemy Info: ID: x:" + std::to_string(TileId().first) + " y: " + std::to_string(TileId().second)
		+ "\nPos x : " + Utils::FormatFloat(pos.x, 2) + " y: " + Utils::FormatFloat(pos.y, 2)
		+ "\nSize x: " + Utils::FormatFloat(size.x, 2) + " y: " + Utils::FormatFloat(size.x, 2)
		+ "\nHP: " + std::to_string(attribute.Health())
		+ "\nDamage Multiplier: " + std::to_string(attribute.DamageMultiplier())
		+ "\nIsGrounded: " + std::string(physics.IsGrounded() ? "true" : "false");
}