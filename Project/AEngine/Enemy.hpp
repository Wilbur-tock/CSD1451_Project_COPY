#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "pch.hpp"

#include "Attributes.hpp"
#include "GameObject.hpp"
#include "Physics.hpp"

enum EnemyType
{
	MINION,
	BOSS,
};

class Enemy : public GameObject {
public:
	Attributes attribute;
	Physics physics;

	Enemy(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, std::pair<int, int> _tileId);

	EnemyType Type() const;

	std::pair<int, int> TileId() const;
	void TileId(std::pair<int, int> newId);

	void Update();
	void Render();
	void DebugInfo(std::string &str);

private:
	EnemyType type;
	std::pair<int, int> tileId; // tile x and y index
};

#endif // !ENEMY_HPP
