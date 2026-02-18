#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "pch.hpp"

#include "Attributes.hpp"
#include "GameObject.hpp"

class Enemy : public GameObject {
public:
	Attributes attribute;

	Enemy() = default;
	Enemy(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture);

	void HealthBarUI();
private:
};

#endif // !ENEMY_HPP
