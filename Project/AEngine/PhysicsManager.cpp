#include "PhysicsManager.hpp"

#include "BaseTypes.hpp"

namespace PhysicsManager {
	float g_dt = 0.0f;

    void ResolveHorizontalCollision(Transform& obj, AEVec2& velocity, Transform const& tileTransform) {
        float halfW = obj.size.x * 0.5f;
        float bounce = -0.1f;
        float bias = 0.01f;

        float halfTileW = tileTransform.size.x * 0.5f;
        float tileLeft = tileTransform.pos.x - halfTileW;
        float tileRight = tileTransform.pos.x + halfTileW;

        if (obj.pos.x < tileTransform.pos.x) { // Hit left side bounce off from the left side to move right
            obj.pos.x = tileLeft - halfW - bias;
            if (velocity.x > 0) velocity.x *= bounce;
        }
        else { // hit right side bounce off from the right side to move left
            obj.pos.x = tileRight + halfW + bias;
            if (velocity.x < 0) velocity.x *= bounce;
        }
    }

    void ResolveVerticalCollision(Transform& obj, AEVec2& velocity, bool& isGrounded, Transform const& tileTransform) {
        float halfH = obj.size.y * 0.5f;
        float halfTileH = tileTransform.size.y * 0.5f;

        float tileTop = tileTransform.pos.y + halfTileH;
        float tileBottom = tileTransform.pos.y - halfTileH;

        float bounce = -0.1f;
        float groundFriction = 0.75f;

        // Hit the top of the block (Floor)
        if (obj.pos.y > tileTransform.pos.y) {
            obj.pos.y = tileTop + halfH; // Flush to the tile
            isGrounded = true;

            velocity.y *= bounce;      // Vertical bounce/energy loss
            velocity.x *= groundFriction; // Horizontal friction

            // Sleep check: prevent infinite micro-bouncing
            if (std::abs(velocity.y) < Config::MINIMUM_VELOCITY) {
                velocity.y = 0;
            }
        }
        // Hit the bottom of the block (Ceiling)
        else {
            obj.pos.y = tileBottom - halfH;
            velocity.y *= bounce;
        }
    }
}