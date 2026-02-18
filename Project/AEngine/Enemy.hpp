#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "pch.hpp"

#include "Attributes.hpp"
#include "GameObject.hpp"

enum EnemyType
{
	MINION,
	BOSS,
};

class Enemy : public GameObject {
public:
	Attributes attribute;

	Enemy() = default;
	Enemy(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, std::pair<int, int> _tileId);

	EnemyType Type() const;
	std::pair<int, int> TileId() const;

	void HealthBarUI();
private:
	EnemyType type;
	std::pair<int, int> tileId; // tile x and y index
};

#endif // !ENEMY_HPP
