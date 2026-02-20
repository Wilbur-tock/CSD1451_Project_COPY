#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "pch.hpp"

#include "Attributes.hpp"
#include "GameObject.hpp"
#include "Physics.hpp"
#include "TileIndex.hpp"

enum EnemyType
{
	MINION,
	BOSS,
};

class Enemy : public GameObject {
public:
	Attributes attribute;
	Physics physics;

	Enemy(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId);

	EnemyType Type() const;

	void TakeDamage(int damage);

	void Update();
	void Render();
	void DebugInfo(std::string &str);

private:
	EnemyType type;
};

#endif // !ENEMY_HPP
