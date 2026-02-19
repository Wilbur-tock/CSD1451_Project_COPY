#include "Projectile.hpp"

#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Enemy.hpp"

Projectile::Projectile() : timer{ 0.0f }, velocity{ 0.0f,0.0f } {}
Projectile::Projectile(AEGfxTexture* _texture, ProjectileMode _mode, AEVec2 _pos, int _attackCharges, AEVec2 _velocity)
    : GameObject(_pos, defaultSize, _texture), velocity(_velocity), timer{ 0.0f }, mode(_mode), attackCharges{_attackCharges} {
}

int  Projectile::AttackCharges() const { return attackCharges; }
void Projectile::AttackCharges(int value) { attackCharges = std::clamp(value, 0, 100); }

ProjectileMode Projectile::Mode() const { return mode; }
void Projectile::Mode(ProjectileMode newMode) { mode = newMode; }

int Projectile::ProjectileId() const { return projectileId; }

void Projectile::AbilityUpdate() {
    if (Mode() == IDLE || Mode() == ATTACK) return;

    if (Mode() == BOMB) {
        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            velocity = { 0,0 }; // fall straight down immediately
            std::cout << "BOMB mode triggered";
        }
    }
}

void Projectile::Render() {
    AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, 0, size.x, size.y);
    GraphicsManager::DrawGameObject(texture, mat, Color{ 0.5f, 0.5f, 0, 1 });
}

void Projectile::MoveProjectile(float dt) {
    if (isGrounded) {
        velocity.x *= pow(Config::FRICTION, dt);
        velocity.y = 0; // ensure that the projectile stay on the ground
        if (std::abs(velocity.x) < Config::MINIMUM_VELOCITY) velocity.x = 0;
    }
    else {
        velocity.y -= Config::GRAVITY * dt;
    }

    pos.x += velocity.x * dt;
    pos.y += velocity.y * dt;
}

void Projectile::SimulatePhysics(Tile const& tile) {
    float bias = 0.1f, radius = size.y * 0.5f;
    // Bounds
    float tileTop = tile.pos.y + (tile.size.y / 2.0f);
    float tileBottom = tile.pos.y - (tile.size.y / 2.0f);
    float tileLeft = tile.pos.x - (tile.size.x / 2.0f);
    float tileRight = tile.pos.x + (tile.size.x / 2.0f);

    float overlapX = (radius + tile.size.x / 2.0f) - std::abs(pos.x - tile.pos.x);
    float overlapY = (radius + tile.size.y / 2.0f) - std::abs(pos.y - tile.pos.y);

    if (overlapX < (overlapY + bias)) {
        float bounce = -0.1f;
        // Resolve X (Wall) - Hit a side wall
        if (pos.x < tile.pos.x) {
            pos.x = tileLeft - radius - 0.1f; // Nudge out
            if (velocity.x > 0) velocity.x *= bounce;
        }
        else {
            pos.x = tileRight + radius + 0.1f; // Nudge out
            if (velocity.x < 0) velocity.x *= bounce;
        }
    }
    else {
        // Resolve Y (Floor/Ceiling) - Hit the top or bottom of the block
        if (pos.y > tile.pos.y) {
            pos.y = tileTop + radius; // Flush to the tile
            isGrounded = true;
            velocity.y *= -0.1f; // Lose 80% of its velocity
            velocity.x *= 0.9f; // ADD FRICTION
            if (std::abs(velocity.y) < Config::MINIMUM_VELOCITY) velocity.y = 0; // prevent tiny velocity
        }
        else { // If it hits the celling fall down
            pos.y = tileBottom - radius;
            velocity.y *= -0.1f; // Lost 60% of its velocity
        }
    }
}

void Projectile::ConsumeAttackCharge() {
    if (--attackCharges <= 0) {
        attackCharges = 0;
        mode = IDLE;
    }
}

void Projectile::Destroy() {
    isActive = isCollisionDetected = toBeDestroyed = false;
    timer = 0.0f;
}

void Projectile::DamageTile(int x, int y) {
    if (Mode() == IDLE) return;

    int blastRadius = attribute.DamageRadius();

    std::cout << "[Projectile] Damage Radius: " << blastRadius << " Center: " << x << " " << y << std::endl;

    int minDX = x, minDY = y, maxDX = x, maxDY = y;
    // Increased damage radius based on blast radius
    if (blastRadius > 1) {
        int addRadius = blastRadius - 1;
        minDX -= addRadius;
        minDY -= addRadius;
        maxDX += addRadius;
        maxDY += addRadius;
    }

    // Damage how many number of tile based on the radius using a nested for loop
    for (int dY = minDY; dY <= maxDY; ++dY) {
        for (int dX = minDX; dX <= maxDX; ++dX) {
            if (Utils::IsGridIndexOutOfBounds(dX, dY)) continue;

            std::cout << "[Projectile] Tiles checked " << dX << " " << dY << std::endl;

            Tile& tileToDamage = (*MapManager::currentMap)[dY][dX];
            // Only damage blocks or enemy tile
            if (!tileToDamage.IsBlocks() && !tileToDamage.IsEnemy()) continue;

            // Damage tile blocks
            std::cout << "[Projectile] Damage Tile: " << TileTypeToString(tileToDamage.Type())
                << " x: " << tileToDamage.gridId.x << " y: " << tileToDamage.gridId.y
                << " " << tileToDamage.attribute.Health() << std::endl;

            tileToDamage.TakeDamage(attribute.Damage());

            // Damage Enemy Object
            if (tileToDamage.IsEnemy()) {
                tileToDamage.enemy->attribute.TakeDamage(attribute.Damage());

                std::cout << "[Projectile] Damaged Enemy" << std::endl;
            }
        }
    }

    ConsumeAttackCharge();
}