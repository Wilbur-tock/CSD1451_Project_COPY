#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "pch.hpp"
#include "Attributes.hpp"
#include "GameObject.hpp" // Using GameObject and TileIndex.hpp
#include "Physics.hpp"

class Player : public GameObject {
public:
	Attributes attribute;
	Physics physics;

	Player(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId);
private:
};

#endif // !PLAYER_HPP
