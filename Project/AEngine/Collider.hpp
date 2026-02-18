#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "pch.hpp"

enum ShapeType { COLLIDER_BOX, COLLIDER_CIRCLE };

class Collider {
public:
	ShapeType type = COLLIDER_BOX;
	AEVec2 size{ 0,0 };
	AEVec2 offset{ 0,0 }; //offset from the center

	Collider(ShapeType _type, AEVec2 _size, AEVec2 _offset = { 0,0 });
};


#endif // !COLLIDER_HPP
