#include "GameObject.hpp"

GameObject::GameObject(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId, Color _textureColor) 
	: id{ 0 }, tileId{ _tileId }, transform{ _pos, _size }, velocity{0,0}, texture{ _texture }, textureColor{ _textureColor }, collider{ CollisionType::COLLIDER_BOX, _size } {
}

bool GameObject::IsInMapBounds() const {
	return transform.pos.x >= Config::WORLD_MIN_X && transform.pos.x <= Config::WORLD_MAX_X 
		&& transform.pos.y >= Config::WORLD_MIN_Y && transform.pos.y <= Config::WORLD_MAX_Y;
}

bool GameObject::IsInMapBoundsX() const {
	return transform.pos.x >= Config::WORLD_MIN_X && transform.pos.x <= Config::WORLD_MAX_X;
}

bool GameObject::IsInMapBoundsY() const {
	return transform.pos.y >= Config::WORLD_MIN_Y && transform.pos.y <= Config::WORLD_MAX_Y;
}