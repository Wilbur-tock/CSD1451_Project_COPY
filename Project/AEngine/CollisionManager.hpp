#ifndef COLLISIONMANAGER_HPP
#define COLLISIONMANAGER_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Collider.hpp"

namespace CollisionManager {
	bool AreCirclesIntersecting(float x1, float y1, float r1, float x2, float y2, float r2);

	bool AreCircleSquareIntersecting(float sX, float sY, float sWidth, float sHeight, float cX, float cY, float r);
	// Checking if MOUSE WORLD COORINDATES is inside the game object
	bool IsPointInGameObject(GameObject const& obj);
	// Checking if world mouse position is inside a rectangle game object
	bool IsPointInRect(float x, float y, float w, float h);
}

#endif // !COLLISIONMANAGER_HPP
