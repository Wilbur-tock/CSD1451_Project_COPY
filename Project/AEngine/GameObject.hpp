#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "pch.hpp"

#include "Color.hpp"

class GameObject {
public:
	int id = 0; // index of the object
	AEVec2 pos;
	AEGfxTexture* texture; // Image texture to drawn
	Color textureColor;
	// Add collider here

	GameObject() = default;
	GameObject(AEVec2 _pos, AEGfxTexture* _texture, Color _textureColor = Color{0,0,0});
};

#endif // GAMEOBJECT_HPP
