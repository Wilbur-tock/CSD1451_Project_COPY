#include "GameObject.hpp"

GameObject::GameObject(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, Color _textureColor) 
	: pos{ _pos }, size{_size}, texture {_texture}, textureColor{ _textureColor } {
}