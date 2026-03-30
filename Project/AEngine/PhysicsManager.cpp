/*
Authors: Dayrius
Description: Manage physics of the game and check for collision to trigger physics

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "PhysicsManager.hpp"

#include "BaseTypes.hpp"
#include "Utils.hpp"
#include "MapManager.hpp"

namespace PhysicsManager {
	float g_dt = 0.0f;

    void PerfectShotCalculation(Transform const& current, Transform const& target, float& cannonAngle, float& shotPower) {
        float dx = target.pos.x - current.pos.x;
        float dy = target.pos.y - current.pos.y;

        float absDx = fabsf(dx);
        bool  shootLeft = (dx < 0);

        float minAngle = 30.0f * (PI / 180.0f);
        float maxAngle = 60.0f * (PI / 180.0f);
        float angle = Utils::RandomValue(minAngle, maxAngle);

        float cosAngle = cosf(angle);
        float tanAngle = tanf(angle);

        float numerator = PhysicsManager::GRAVITY * absDx * absDx;
        float denom = 2.0f * cosAngle * cosAngle * (absDx * tanAngle - dy);

        // Find a new angle if denominator is 0
        if (denom <= 0.0f) {
            angle = 75.0f * (PI / 180.0f);
            cosAngle = cosf(angle);
            tanAngle = tanf(angle);
            denom = 2.0f * cosAngle * cosAngle * (absDx * tanAngle - dy);
        }

        shotPower = sqrtf(numerator / denom);

        // Apply direction
        cannonAngle = shootLeft ? (PI - angle) : angle;
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

    bool IsObjectGrounded(Transform const& trans) {
        int tileX = 0, tileY = 0;
        Utils::GetTileIndex(tileX, tileY, trans.pos);

        int maxY = tileY + 1;
        int minX = tileX - 1;
        int maxX = tileX + 1;

        // 010 
        // 000
        // Checks 2x3 with 1 - Projectile, 0 - tiles around the projectile

        for (int y = tileY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                if (Utils::IsTileIndexOutOfBounds(x, y)) continue;

                Tile& tile = (*MapManager::currentMap)[y][x];

                if (!tile.IsEmpty()) {
                    if (IsGrounded(trans, tile.transform)) return true;
                }
            }
        }

        return false;
    }
}