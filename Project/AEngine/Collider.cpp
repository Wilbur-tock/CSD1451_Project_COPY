#include "Collider.hpp"

Collider::Collider(CollisionType _type, AEVec2 _size, AEVec2 _offset) 
	: type{ _type }, size{_size}, offset{_offset} {}