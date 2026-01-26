#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "pch.hpp"

class GameObject {
public:
	int id = 0; // index of the object
	AEVec2 pos;
	AEGfxTexture* texture; // Image texture to drawn
	// Add collider here

	GameObject() = default;
	GameObject(AEVec2 _pos, AEGfxTexture* _texture);
};

#endif // GAMEOBJECT_HPP
