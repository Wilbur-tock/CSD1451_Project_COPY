/*
Authors: Dayrius
Description: Collider class to store collision data

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "Collider.hpp"

Collider::Collider(CollisionType _type, AEVec2 _size, AEVec2 _offset) 
	: type{ _type }, size{_size}, offset{_offset} {}