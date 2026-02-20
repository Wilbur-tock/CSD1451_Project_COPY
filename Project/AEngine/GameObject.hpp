#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "pch.hpp"

#include "Color.hpp"
#include "Collider.hpp"

class GameObject {
public:
	int id = 0; // index of the object
	AEVec2 pos{ 0,0 };
	AEVec2 size{ 0,0 };
	AEVec2 velocity = { 0,0 }; // Vector rate of change of displacement (direction * speed)

	AEGfxTexture* texture = nullptr; // Image texture to drawn
	Color textureColor{ 0,0,0 };
	Collider collider{ COLLIDER_BOX, {0,0} };

	GameObject() = default;
	GameObject(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, Color _textureColor = Color{0,0,0});

	bool IsInMapBounds() const;
};

#endif // GAMEOBJECT_HPP
