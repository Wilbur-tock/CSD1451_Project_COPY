#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "pch.hpp"

#include "Color.hpp"
#include "Collider.hpp"
#include "TileIndex.hpp"
#include "BaseTypes.hpp"

class GameObject {
public:
	int id = 0; // index of the object
	TileIndex tileId{ 0,0 }; // x and y tile index of where the gameobject is position according to the tile map
	Transform transform{ {0,0}, { 0,0 } };
	AEVec2 velocity = { 0,0 }; // Vector rate of change of displacement (direction * speed)

	AEGfxVertexList* pMesh = nullptr;
	AEGfxTexture* texture = nullptr; // Image texture to drawn
	Color textureColor{ 0,0,0 };
	Collider collider{ CollisionType::COLLIDER_BOX, {0,0} };

	// CTOR 
	GameObject() = default;
	GameObject(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId = { 0,0 }, Color _textureColor = Color{ 0,0,0 });
	GameObject(GameObject const& other);
	GameObject& operator=(GameObject const& other);
	~GameObject();

	bool IsInMapBounds() const;
	bool IsInMapBoundsX() const;
	bool IsInMapBoundsY() const;
};

#endif // GAMEOBJECT_HPP
