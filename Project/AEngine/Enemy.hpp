#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "pch.hpp"

#include "Attributes.hpp"
#include "GameObject.hpp" // Using GameObject and TileIndex.hpp
#include "Physics.hpp"

enum class EnemyType
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

	void Update();
	void Render();
	void DebugInfo(std::string &str);

private:
	EnemyType type;
};

#endif // !ENEMY_HPP
