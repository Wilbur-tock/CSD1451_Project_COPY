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
    float objHalfW = obj.transform.size.x * 0.5f;
    float objHalfH = obj.transform.size.y * 0.5f;

    float objMinX = obj.transform.pos.x - objHalfW;
    float objMaxX = obj.transform.pos.x + objHalfW;
    float objMinY = obj.transform.pos.y - objHalfH;
    float objMaxY = obj.transform.pos.y + objHalfH;

    // Tile bounds
    float tileHalfW = belowTile.transform.size.x * 0.5f;
    float tileHalfH = belowTile.transform.size.y * 0.5f;

    float tileMinX = belowTile.transform.pos.x - tileHalfW;
    float tileMaxX = belowTile.transform.pos.x + tileHalfW;
    float tileMinY = belowTile.transform.pos.y - tileHalfH;
    float tileMaxY = belowTile.transform.pos.y + tileHalfH;

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
        obj.velocity.y = Config::GRAVITY * PhysicsManager::g_dt * 0.1f;
        obj.transform.pos.y -= obj.velocity.y;

        // Snap to the bottom tile
        if (obj.transform.pos.y <= btmTile.transform.pos.y) {
            obj.transform.pos.y = btmTile.transform.pos.y;
            
            Tile& currTile = *MapManager::FindTile(obj.tileId);

            // Swap tiles details
            btmTile.Type(currTile);
            currTile.Type(TILE_EMPTY); // Maybe change to other tile type
            obj.tileId = btmTileId;
        }
    }
}