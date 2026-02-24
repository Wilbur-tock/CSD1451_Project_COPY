#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "pch.hpp"

enum class CollisionType { COLLIDER_BOX, COLLIDER_CIRCLE };

class Collider {
public:
	CollisionType type = CollisionType::COLLIDER_BOX;
	AEVec2 size{ 0,0 };
	AEVec2 offset{ 0,0 }; //offset from the center

	Collider(CollisionType _type, AEVec2 _size, AEVec2 _offset = { 0,0 });
};


#endif // !COLLIDER_HPP
