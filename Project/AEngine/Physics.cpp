#include "Physics.hpp"

#include "PhysicsManager.hpp"
#include "MapManager.hpp"
#include "Utils.hpp"

bool Physics::IsGrounded() const { return isGrounded; }

void Physics::GroundedChecks(GameObject const& obj) {
    TileIndex btmId{ obj.tileId.x, obj.tileId.y + 1 };
    if (btmId.IsOutOfBounds()) {
        isGrounded = true;
        return;
    }
    Tile const& belowTile = *MapManager::FindTile(btmId);

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

    // AABB collision 
    bool overlapX = (objMaxX > tileMinX) && (objMinX < tileMaxX);
    bool overlapY = (objMaxY > tileMinY) && (objMinY <= tileMaxY);

    if (overlapX && overlapY && !belowTile.IsEmpty()) {
        isGrounded = true;
    }
    else {
        isGrounded = false;
    }
}

void Physics::Update(GameObject& obj) {
    GroundedChecks(obj);

    if (!isGrounded) {
        TileIndex btmTileId = { obj.tileId.x, obj.tileId.y + 1 };
        if (btmTileId.IsOutOfBounds()) {
            return;
        }

        Tile& btmTile = *MapManager::FindTile(btmTileId);
        // Gravity pushing the obj down to the bottom tile (ADDED 0.1f to ensure that is slowly glide down)
        obj.velocity.y = Config::GRAVITY * PhysicsManager::dt * 0.05f;
        obj.pos.y -= obj.velocity.y;

        // Snap to the bottom tile
        if (obj.pos.y <= btmTile.pos.y) {
            obj.pos.y = btmTile.pos.y;
            
            Tile& currTile = *MapManager::FindTile(obj.tileId);

            // Swap tiles details
            btmTile.Type(currTile);
            currTile.Type(TILE_EMPTY);
            obj.tileId = btmTileId;
        }
    }
}