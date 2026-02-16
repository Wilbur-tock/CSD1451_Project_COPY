#include "Projectile.hpp"

#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Tile.hpp"

Projectile::Projectile() : timer{ 0.0f }, velocity{ 0.0f,0.0f } {}
Projectile::Projectile(AEVec2 _pos, AEGfxTexture* _texture, AEVec2 _velocity)
    : GameObject(_pos, defaultSize, _texture), timer{ 0.0f }, velocity(_velocity) {
}

int  Projectile::AttackCharges() const { return attackCharges; }
void Projectile::AttackCharges(int value) { attackCharges = std::clamp(value, 0, 100); }

ProjectileMode Projectile::Mode() const { return mode; }
void Projectile::Mode(ProjectileMode newMode) { mode = newMode; }

void Projectile::Render() {
    AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, 0, size.x, size.y);
    GraphicsManager::DrawGameObject(AssetsManager::circleTexture, mat, Color{ 0.5f, 0.5f, 0, 1 });
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

void Projectile::ConsumeAttackCharge() {
    if (--attackCharges < 0) {
        attackCharges = 0;
        mode = IDLE;
    }
}

void Projectile::Destroy() {
    isActive = isCollisionDetected = toBeDestroyed = false;
    timer = 0.0f;
}

void Projectile::DamageBlocks(int x, int y, int damage, int blastRadiusMod) {
    if (Mode() == IDLE) return;

    int minDX = x, minDY = y, maxDX = x, maxDY = y;
    // Increased damage radius based on blast radius
    if (blastRadiusMod > 1) {
        int addRadius = blastRadiusMod - 1;
        minDX -= addRadius;
        minDY -= addRadius;
        maxDX += addRadius;
        maxDY += addRadius;
    }

    for (int dY = minDY; dY <= maxDY; ++dY) {
        for (int dX = minDX; dX <= maxDX; ++dX) {
            if (Utils::IsGridIndexOutOfBounds(dX, dY)) continue;

            Tile& tileToDamage = (*MapManager::currentMap)[dY][dX];
            // Only damage blocks type
            if (!tileToDamage.IsBlocks()) continue;

            std::cout << "Damage Tile: " << TileTypeToString(tileToDamage.Type())
                << " x: " << tileToDamage.gridId.x << " y: " << tileToDamage.gridId.y
                << " " << tileToDamage.attributes.Health() << std::endl;

            tileToDamage.Damage(damage);
            ConsumeAttackCharge();
        }
    }
}