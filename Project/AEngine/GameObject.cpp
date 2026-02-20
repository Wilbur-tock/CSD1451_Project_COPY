#include "GameObject.hpp"

GameObject::GameObject(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, Color _textureColor) 
	: id{ 0 }, pos{ _pos }, size{ _size }, velocity{0,0}, texture{ _texture }, textureColor{ _textureColor }, collider{ COLLIDER_BOX, _size } {
}

bool GameObject::IsInMapBounds() const {
	using namespace Config;
	return pos.x >= WORLD_MIN_X && pos.x <= WORLD_MAX_X && pos.y >= WORLD_MIN_Y && pos.y <= WORLD_MAX_Y;
}