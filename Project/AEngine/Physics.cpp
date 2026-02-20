#include "Physics.hpp"

#include "PhysicsManager.hpp"
#include "MapManager.hpp"
#include "Utils.hpp"

bool Physics::IsGrounded() const { return isGrounded; }

void Physics::GroundedChecks(GameObject const& obj,std::pair<int, int>& tileId) {
    int belowX = tileId.first, belowY = tileId.second + 1;
    if (Utils::IsGridIndexOutOfBounds(belowX, belowY)) {
        isGrounded = true;
        return;
    }
    Tile& belowTile = (*MapManager::currentMap)[belowY][belowX];

    // Object Bounds
    float objHalfW = obj.size.x * 0.5f;
    float objHalfH = obj.size.y * 0.5f;

    float objMinX = obj.pos.x - objHalfW;
    float objMaxX = obj.pos.x + objHalfW;
    float objMinY = obj.pos.y - objHalfH;
    float objMaxY = obj.pos.y + objHalfH;

    // Tile bounds
    float tileHalfW = belowTile.size.x * 0.5f;
    float tileHalfH = belowTile.size.y * 0.5f;

    float tileMinX = belowTile.pos.x - tileHalfW;
    float tileMaxX = belowTile.pos.x + tileHalfW;
    float tileMinY = belowTile.pos.y - tileHalfH;
    float tileMaxY = belowTile.pos.y + tileHalfH;

    bool overlapX = (objMaxX > tileMinX) && (objMinX < tileMaxX);
    bool overlapY = (objMaxY > tileMinY) && (objMinY <= tileMaxY);

    if (overlapX && overlapY && !belowTile.IsEmpty()) {
        isGrounded = true;
    }
    else {
        isGrounded = false;
    }
}

void Physics::Update(GameObject& obj, std::pair<int, int>& tileId) {
    GroundedChecks(obj, tileId);

    if (!isGrounded) {
        int belowX = tileId.first, belowY = tileId.second + 1;
        if (Utils::IsGridIndexOutOfBounds(belowX, belowY)) {
            return;
        }

        Tile& btmTile = (*MapManager::currentMap)[belowY][belowX];
        // Gravity pushing the obj down to the bottom tile (ADDED 0.1f to ensure that is slowly glide down)
        obj.velocity.y = Config::GRAVITY * PhysicsManager::dt * 0.1f;
        obj.pos.y -= obj.velocity.y;

        // Snap to the bottom tile
        if (obj.pos.y <= btmTile.pos.y) {
            obj.pos.y = btmTile.pos.y;

            ++tileId.second; // update to the next tile index
        }
    }
}