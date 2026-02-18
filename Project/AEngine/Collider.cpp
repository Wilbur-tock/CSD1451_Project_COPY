#include "Collider.hpp"

Collider::Collider(ShapeType _type, AEVec2 _size, AEVec2 _offset) 
	: type{ _type }, size{_size}, offset{_offset} {}