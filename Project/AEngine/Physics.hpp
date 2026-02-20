#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "pch.hpp"
#include "GameObject.hpp"

class Physics {
public:
	bool IsGrounded() const;

	void GroundedChecks(GameObject const& obj, std::pair<int, int>& tileId);

	void Update(GameObject& obj, std::pair<int, int>& tileId);
private:
	bool isGrounded = false;
};

#endif // !PHYSICS_HPP
