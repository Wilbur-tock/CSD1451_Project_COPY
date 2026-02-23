#ifndef COLLISIONMANAGER_HPP
#define COLLISIONMANAGER_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Collider.hpp"

namespace CollisionManager {
	bool AreCirclesIntersecting(float x1, float y1, float r1, float x2, float y2, float r2);

	bool AreCircleSquareIntersecting(float sX, float sY, float sWidth, float sHeight, float cX, float cY, float r);
    /*!***********************************************************************
    \brief
    Checking if MOUSE WORLD COORINDATES is inside the game object collider
    \param[in] obj
    The game object collider to check
    \return
    return true if mouse is inside the gameobject collider otherwise false
    *************************************************************************/
	bool IsPointInGameObject(GameObject const& obj);
    /*!***********************************************************************
    \brief
    Checking if world mouse position is inside a rectangle object
    \param[in] x,y
    x and y position of the rectangle
    \param[in] w,h
    width and height of the rectangle
    \return
    return true if mouse is inside the rectangle otherwise false
    *************************************************************************/
	bool IsPointInRect(float x, float y, float w, float h);
}

#endif // !COLLISIONMANAGER_HPP
