#include "Player.hpp"

Player::Player(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId) 
	: GameObject{ _pos,_size, _texture, _tileId } {}