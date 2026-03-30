/*
Authors: Dayrius
Description: Stores physics data

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "pch.hpp"
#include "GameObject.hpp"
#include "TileIndex.hpp"

class Physics {
public:
	bool IsGrounded() const { return isGrounded; }

	void GroundedChecks(GameObject const& obj);
	void Update(GameObject& obj);
private:
	bool isGrounded = false;
};

#endif // !PHYSICS_HPP
