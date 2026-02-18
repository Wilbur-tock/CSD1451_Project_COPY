#include "Enemy.hpp"

#include "UIManager.hpp"

Enemy::Enemy(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture) 
	: GameObject{ _pos, _size, _texture, Color{1,1,1} } {
}

void Enemy::HealthBarUI() {
	attribute.Health(attribute.Health() - 1);
	if (attribute.Health() <= 0) attribute.Health(100);

	AEVec2 barPos = { pos.x, pos.y + size.y };
	AEVec2 barSize = { size.x, 10 };

	UIManager::DrawBar({ barPos, barSize, Color{1,0,0} },
		static_cast<float>(attribute.Health()), static_cast<float>(attribute.MaxHealth()));
}