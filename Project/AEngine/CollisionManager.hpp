#ifndef COLLISIONMANAGER_HPP
#define COLLISIONMANAGER_HPP

#include "pch.hpp"

namespace CollisionManager {
	bool AreCirclesIntersecting(float x1, float y1, float r1, float x2, float y2, float r2);

	// Checking if world mouse position is inside a rectangle game object
	bool IsPointInRect(float x, float y, float w, float h);
}

#endif // !COLLISIONMANAGER_HPP
