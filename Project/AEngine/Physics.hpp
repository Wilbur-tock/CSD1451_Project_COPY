#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "pch.hpp"
#include "GameObject.hpp"
#include "TileIndex.hpp"

class Physics {
public:
	bool IsGrounded() const;

	void GroundedChecks(GameObject const& obj, TileIndex& tileId);

	void Update(GameObject& obj, TileIndex& tileId);
private:
	bool isGrounded = false;
};

#endif // !PHYSICS_HPP
