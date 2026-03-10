#include "Physics.hpp"

#include "PhysicsManager.hpp"
#include "MapManager.hpp"
#include "Utils.hpp"

void Physics::GroundedChecks(GameObject const& obj) {
    TileIndex btmId{ obj.tileId.x, obj.tileId.y + 1 };
    if (btmId.IsOutOfBounds()) {
        isGrounded = true;
        return;
    }
    Tile& btmTile = *MapManager::FindTile(btmId);

    isGrounded = !btmTile.IsEmpty() && PhysicsManager::IsGrounded(obj.transform, btmTile.transform);
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
        obj.velocity.y = PhysicsManager::GRAVITY * PhysicsManager::g_dt * 0.1f;
        obj.transform.pos.y -= obj.velocity.y;

        // Snap to the bottom tile
        if (obj.transform.pos.y <= btmTile.transform.pos.y) {
            obj.transform.pos.y = btmTile.transform.pos.y;
            
            Tile& currTile = *MapManager::FindTile(obj.tileId);

            // Swap tiles details
            btmTile.Type(currTile);
            currTile.Type(TileType::TILE_EMPTY); // Maybe change to other tile type
            obj.tileId = btmTileId;
        }
    }
}