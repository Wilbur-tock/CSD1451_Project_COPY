#include "PhysicsManager.hpp"

#include "BaseTypes.hpp"
#include "Utils.hpp"

namespace PhysicsManager {
	float g_dt = 0.0f;

    void PerfectShotCalculation(Transform const& current, Transform const& target, float& cannonAngle, float& shotPower) {
        float dx = target.pos.x - current.pos.x;
        float dy = target.pos.y - current.pos.y;
        float distance = sqrt(dx * dx + dy * dy);

        // Randomise angle between the min and max radian
        float min = 30 * (PI / 180.0f);
        float max = 60 * (PI / 180.0f);
        float randomAngleBase = Utils::RandomValue(min, max);

        // Adjust for direction (Shooting left or right)
        cannonAngle = (dx > 0) ? randomAngleBase : (PI - randomAngleBase);

        // Calculate required Power (Initial Velocity)
        // Basic range formula: v = sqrt( (g * d) / sin(2 * theta) )
        shotPower = sqrt((PhysicsManager::GRAVITY * abs(distance)) / sin(2 * randomAngleBase));
    }

    void ResolveHorizontalCollision(Transform& obj, AEVec2& velocity, Transform const& tileTransform) {
        float halfW = obj.size.x * 0.5f;

        float halfTileW = tileTransform.size.x * 0.5f;
        float tileLeft = tileTransform.pos.x - halfTileW;
        float tileRight = tileTransform.pos.x + halfTileW;

        if (obj.pos.x < tileTransform.pos.x) { // Hit left side of a block bounce off from the left side to move right
            obj.pos.x = tileLeft - halfW - BIAS;
            if (velocity.x > 0) velocity.x *= BOUNCE;
        }
        else { // hit right side of a block bounce off from the right side to move left
            obj.pos.x = tileRight + halfW + BIAS;
            if (velocity.x < 0) velocity.x *= BOUNCE;
        }
    }

    void ResolveVerticalCollision(Transform& obj, AEVec2& velocity, bool& isGrounded, Transform const& tileTransform) {
        float halfH = obj.size.y * 0.5f;
        float halfTileH = tileTransform.size.y * 0.5f;

        float tileTop = tileTransform.pos.y + halfTileH;
        float tileBottom = tileTransform.pos.y - halfTileH;

        // Hit the top of the block (Floor)
        if (obj.pos.y > tileTransform.pos.y) {
            obj.pos.y = tileTop + halfH; // Flush to the tile
            isGrounded = true;

            velocity.y *= BOUNCE;      // Vertical bounce/energy loss
            velocity.x *= 1.0f; // Horizontal friction

            // Sleep check: prevent infinite micro-bouncing
            if (std::abs(velocity.y) < MINIMUM_VELOCITY) {
                velocity.y = 0;
            }
        }
        // Hit the bottom of the block (Ceiling)
        else {
            obj.pos.y = tileBottom - halfH;
            velocity.y *= BOUNCE;
        }
    }

    bool IsGrounded(Transform const& obj, Transform const& tile) {
        // Object Bounds
        float objHalfW = obj.size.x * 0.5f;
        float objHalfH = obj.size.y * 0.5f;

        float objMinX = obj.pos.x - objHalfW;
        float objMaxX = obj.pos.x + objHalfW;
        float objMinY = obj.pos.y - objHalfH;
        float objMaxY = obj.pos.y + objHalfH;

        // Tile bounds
        float tileHalfW = tile.size.x * 0.5f;
        float tileHalfH = tile.size.y * 0.5f;

        float tileMinX = tile.pos.x - tileHalfW;
        float tileMaxX = tile.pos.x + tileHalfW;
        float tileMinY = tile.pos.y - tileHalfH;
        float tileMaxY = tile.pos.y + tileHalfH;

        // AABB collision 
        bool overlapX = (objMaxX > tileMinX) && (objMinX < tileMaxX);
        bool overlapY = (objMaxY > tileMinY) && (objMinY <= tileMaxY);

        return overlapX && overlapY;
    }
}