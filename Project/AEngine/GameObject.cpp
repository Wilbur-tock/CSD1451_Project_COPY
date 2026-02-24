#include "GameObject.hpp"

GameObject::GameObject(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId, Color _textureColor) 
	: id{ 0 }, tileId{ _tileId }, transform{ _pos, _size }, velocity{0,0}, texture{ _texture }, textureColor{ _textureColor }, collider{ CollisionType::COLLIDER_BOX, _size } {
}

bool GameObject::IsInMapBounds() const {
	using namespace Config;
	return transform.pos.x >= WORLD_MIN_X && transform.pos.x <= WORLD_MAX_X && transform.pos.y >= WORLD_MIN_Y && transform.pos.y <= WORLD_MAX_Y;
}