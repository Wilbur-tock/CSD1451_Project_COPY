#ifndef PHYSICSMANAGER_HPP
#define PHYSICSMANAGER_HPP

#include "pch.hpp"
struct Transform; // To be defined in Basetypes.hpp

namespace PhysicsManager {
	extern float g_dt; // Delta time

	void ResolveHorizontalCollision(Transform& obj, AEVec2& velocity, Transform const& tileTransform);
	void ResolveVerticalCollision(Transform& obj, AEVec2& velocity, bool& isGrounded, Transform const& tileTransform);
}

#endif // !PHYSICSMANAGER_HPP
