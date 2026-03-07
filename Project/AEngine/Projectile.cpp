#include "Projectile.hpp"

#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "LevelManager.hpp"
#include "Character.hpp"
#include "CollisionManager.hpp"
#include "PhysicsManager.hpp"

Projectile::Projectile(AEGfxTexture* _texture, ProjectileMode _mode, AEVec2 _pos, int _attackCharges)
    : GameObject(_pos, defaultSize, _texture), timer{ 0.0f }, mode(_mode), attackCharges{_attackCharges} {
}

int  Projectile::AttackCharges() const { return attackCharges; }
void Projectile::AttackCharges(int value) { attackCharges = std::clamp(value, 0, 100); }

ProjectileMode Projectile::Mode() const { return mode; }
void Projectile::Mode(ProjectileMode newMode) { mode = newMode; }

int Projectile::ProjectileId() const { return projectileId; }

std::string Projectile::ProjectileName() const { return projectileName; }

void Projectile::Render() {
    AEMtx33 mat = GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, 0, transform.size.x, transform.size.y);
    GraphicsManager::DrawGameObject(texture, mat, Color{ 0.5f, 0.5f, 0, 1 });
}

void Projectile::AbilityUpdate() {
    if (Mode() == ProjectileMode::IDLE || Mode() == ProjectileMode::ATTACK) return;

    if (Mode() == ProjectileMode::BOMB) {
        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            velocity = { 0,0 }; // fall straight down immediately
            std::cout << "BOMB mode triggered";
        }
    }
}

void Projectile::SetProjectileStats(std::string _projectileName, ProjectileModule const& data) {
    projectileName = _projectileName;

    // 1. Set Identity
    projectileId = data.projectileId;
    mode = static_cast<ProjectileMode>(data.projectileId);

    // 2. Set Stats directly from JSON data
    // Total Damage = Base * Multiplier
    float totalDamage = static_cast<float>(data.baseDamage) * data.damageAmp;
    attribute.Damage(static_cast<int>(totalDamage));

    // Set Radius directly
    attribute.DamageRadius(static_cast<int>(data.radius));

    // Set Charges
    AttackCharges(1);
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

    transform.pos.x += velocity.x * dt;
    transform.pos.y += velocity.y * dt;
}

void Projectile::ProjectilePhysics(Tile const& tile) {
    if(CollisionManager::IsHorizontalCollision(transform, tile.transform)) {
        PhysicsManager::ResolveHorizontalCollision(transform, velocity, tile.transform);
    }
    else {
        PhysicsManager::ResolveVerticalCollision(transform, velocity, isGrounded, tile.transform);
    }
}

void Projectile::ConsumeAttackCharge() {
    if (--attackCharges <= 0) {
        attackCharges = 0;
        mode = ProjectileMode::IDLE;
    }
}

void Projectile::Destroy() {
    isActive = collisionTriggered = toBeDestroyed = false;
    timer = 0.0f;
}

void Projectile::DamageTile(int x, int y) {
    if (Mode() == ProjectileMode::IDLE) return;

    int blastRadius = attribute.DamageRadius();

    std::cout << "===================== [Projectile] Damaging tiles ====================="
        << "\n[Projectile] Damage Radius: " << blastRadius << " Center Tile Id: " << x << " " << y 
        << "\n[Projectile] Remaining Attack Charges: " << AttackCharges() << '\n';

    int minDX = x, minDY = y, maxDX = x, maxDY = y;
    // Increased damage radius based on blast radius
    if (blastRadius > 1) {
        int addRadius = blastRadius - 1;
        minDX -= addRadius;
        minDY -= addRadius;
        maxDX += addRadius;
        maxDY += addRadius;
    }

    bool hasDamageAnyTile = false; // Check if the projectile has damage any tiles (Blocks/Character)
    int projDamage = attribute.Damage();

    // Damage how many number of tile based on the radius using a nested for loop
    for (int dY = minDY; dY <= maxDY; ++dY) {
        for (int dX = minDX; dX <= maxDX; ++dX) {
            if (Utils::IsGridIndexOutOfBounds(dX, dY)) continue;

            Tile& tileToDamage = (*MapManager::currentMap)[dY][dX];
            //std::cout << "[Projectile] Checked tile " << tileToDamage << '\n';

            // Only damage blocks or character tile
            if (!tileToDamage.IsBlocks() && !tileToDamage.IsCharacter()) continue;

            // Damage Tiles that contain a character object
            if (tileToDamage.IsCharacter()) {
                Character* character = LevelManager::selectedLevel->FindCharacterObj(tileToDamage);

                if (character) {
                    character->attribute.TakeDamage(projDamage);
                    std::cout << "[Projectile] Damaged character " << character->TypeName() << " new hp: " << character->attribute.Health() << '\n';
                }
            }

            // Damage tile AFTER it has damage a CHARACTER compononent
            tileToDamage.TakeDamage(projDamage);
            std::cout << "[Projectile] Damage Tile: " << tileToDamage << " Current hp: " << tileToDamage.attribute.Health() << '\n';
            hasDamageAnyTile = true;
        }
    }

    // Consume 1 attack charge if projectile has damaged a tile 
    if (hasDamageAnyTile) {
        ConsumeAttackCharge();

        // Damage effect
        Tile& tileToDamage = (*MapManager::currentMap)[y][x];

        GraphicsManager::DrawGameObject(
            AssetsManager::circleTexture,
            GraphicsManager::CreateTransformMatrix(tileToDamage.transform.pos.x, tileToDamage.transform.pos.y, 0,
                (tileToDamage.transform.size.x * 0.5f) * blastRadius, (tileToDamage.transform.size.y * 0.5f) * blastRadius),
            { 0.75,0.0f,0 }
        );
    }
}