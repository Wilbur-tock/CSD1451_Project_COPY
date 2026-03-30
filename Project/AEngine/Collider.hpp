/*
Authors: Dayrius
Description: Collider class to store collision data

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "AEEngine.h"

enum class CollisionType { COLLIDER_BOX, COLLIDER_CIRCLE };

class Collider {
public:
	CollisionType type = CollisionType::COLLIDER_BOX;
	AEVec2 size{ 0,0 }; // size of the collision
	AEVec2 offset{ 0,0 }; //offset from the origin

	Collider(CollisionType _type, AEVec2 _size, AEVec2 _offset = { 0,0 });
};


#endif // !COLLIDER_HPP
