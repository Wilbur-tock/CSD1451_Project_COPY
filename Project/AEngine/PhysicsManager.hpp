/*
Authors: Dayrius
Description: Manage physics of the game and check for collision to trigger physics

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef PHYSICSMANAGER_HPP
#define PHYSICSMANAGER_HPP

#include "pch.hpp"
struct Transform; // To be defined in Basetypes.hpp

namespace PhysicsManager {
	extern float g_dt; // Delta time

	// Physics Constants
	constexpr float GRAVITY = 1500.0f;
	constexpr float FRICTION = 0.05f;
	constexpr float MINIMUM_VELOCITY = 5.0f;
	constexpr float BIAS = 0.01f;
	constexpr float BOUNCE = -0.1f; // Negative value because when moving left x velocity is negative -*- = positve (moving right)

	// Calculate the ideal cannonAngle and shotPower to hit the target
	void PerfectShotCalculation(Transform const& current, Transform const& target, float& cannonAngle, float& shotPower);
	void ResolveHorizontalCollision(Transform& obj, AEVec2& velocity, Transform const& tileTransform);
	void ResolveVerticalCollision(Transform& obj, AEVec2& velocity, bool& isGrounded, Transform const& tileTransform);
	bool IsGrounded(Transform const& obj, Transform const& tile);
	bool IsObjectGrounded(Transform const& trans);
}

#endif // !PHYSICSMANAGER_HPP
