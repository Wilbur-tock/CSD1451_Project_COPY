#include "Enemy.hpp"

#include "UIManager.hpp"

Enemy::Enemy(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, std::pair<int, int> _tileId)
	: GameObject{ _pos, _size, _texture, Color{1,1,1} }, attribute{}, type{MINION}, tileId{_tileId} {
}

EnemyType Enemy::Type() const { return type; }
std::pair<int,int> Enemy::TileId() const { return tileId; }

void Enemy::HealthBarUI() {
	AEVec2 barPos = { pos.x, pos.y + size.y };
	AEVec2 barSize = { size.x, 10 };

	UIManager::DrawBar({ barPos, barSize, Color{1,0,0} },
		static_cast<float>(attribute.Health()), static_cast<float>(attribute.MaxHealth()));
}