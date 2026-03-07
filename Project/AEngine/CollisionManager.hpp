#ifndef COLLISIONMANAGER_HPP
#define COLLISIONMANAGER_HPP

#include "pch.hpp"

class GameObject;   // To be defined in GameObject.cpp
struct Transform;    // To be defined in BaseTypes.cpp

namespace CollisionManager {
    /*!***********************************************************************
    \brief
    Checking if circle object transform is overlapping with square object transform
    \param[in] circle
    Transform containing the size and position of the circle object
    \param[in] square
    Transform containing the size and position of the square object
    \return
    return true if circle is overlapping with square or vice versa otherwise false
    *************************************************************************/
    bool AreCircleSquareIntersecting(Transform const& circle, Transform const& square);
    bool AreCircleSquareIntersecting(float sX, float sY, float sWidth, float sHeight, float cX, float cY, float r);

    /**********************************************
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

    /*!***********************************************************************
    \brief
    Checking if the collision between the circle and square is horizontal or not
    \param[in] circle
    Transform containing the size and position of the circle object
    \param[in] square
    Transform containing the size and position of the square object
    \return
    return true if the collision is horizontal otherwise false for vertical collision
    *************************************************************************/
    bool IsHorizontalCollision(Transform const& circle, Transform const& square);
}

#endif // !COLLISIONMANAGER_HPP
