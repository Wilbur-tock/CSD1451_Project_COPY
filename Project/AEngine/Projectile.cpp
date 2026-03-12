#include "Projectile.hpp"

#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "LevelManager.hpp"
#include "Character.hpp"
#include "CollisionManager.hpp"
#include "PhysicsManager.hpp"
#include "Utils.hpp"
#include "EffectsManager.hpp"

Projectile::Projectile(AEGfxTexture* _texture, ProjectileMode _mode, AEVec2 _pos, int _attackCharges)
    : GameObject(_pos, defaultSize, _texture), timer{ 0.0f }, mode(_mode), attackCharges{_attackCharges} {
}

void Projectile::Render() {
    AEMtx33 mat = GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, 0, transform.size.x, transform.size.y);
    GraphicsManager::DrawGameObject(texture, mat, Color{ 0.5f, 0.5f, 0, 1 });
}

void Projectile::AbilityActivation() {
    if (Mode() == ProjectileMode::IDLE || Mode() == ProjectileMode::ATTACK || isAbilityTriggered) return;

    bool leftClick = AEInputCheckTriggered(AEVK_LBUTTON);

    switch (Mode()) {
    case ProjectileMode::BOMB:
        if (leftClick) {
            isAbilityTriggered = true;
            // Bomb instant explode and damage tile blocks double its radius once activated
            int tileX = 0, tileY = 0;
            Utils::GetTileIndex(tileX, tileY, transform.pos);
            int radius = attribute.DamageRadius();

            if (!Utils::IsGridIndexOutOfBounds(tileX, tileY)) {
                attribute.DamageRadius(radius * 2);
                DamageTile(tileX, tileY);
                Destroy();
            }

            // Reset stats back to default
            attribute.DamageRadius(radius);
            std::cout << "BOMB mode triggered";
        }
        break;
    case ProjectileMode::NUKE:
        if (leftClick) {
            isAbilityTriggered = true;
            velocity = { 0,0 }; // fall straight down immediately
            std::cout << "NUKE mode triggered";
        }
        break;
    case ProjectileMode::PENTA_BURST:
        if (isCollisionTriggered) { // Give a velocity boost once only to continue hitting more stuff
            isAbilityTriggered = true;
            velocity.x *= 2;
            velocity.y *= 2;
        }
        break;
    case ProjectileMode::WALL:
        if (leftClick) {
            isAbilityTriggered = true;

            // Create a 1 tile width wall 2 tiles X index behind current projectile X index
            int tileX = 0, tileY = 0;
            Utils::GetTileIndex(tileX, tileY, transform.pos);
            tileX -= 2; 

            if (!Utils::IsGridIndexOutOfBounds(tileX, tileY)) {
                for (int i = 0; i <= Config::MAX_GRID_Y; ++i) {
                    Tile& tile = (*MapManager::currentMap)[i][tileX];
                    if (tile.Type() == TileType::TILE_EMPTY) {
                        tile.Type(TileType::TILE_BLOCK_1);
                        tile.attribute.Health(1);
                    }
                }
            }
            std::cout << "WALL mode triggered";
        }
        break;
    }
}

void Projectile::SetProjectileStats(std::string _projectileName, ProjectileModule const& data) {
    projectileName = _projectileName;

    // 1. Set Identity
    projectileId = data.projectileId;
    mode = static_cast<ProjectileMode>(data.projectileId + 1);

    // 2. Set Stats directly from JSON data
    // Total Damage = Base * Multiplier
    float totalDamage = static_cast<float>(data.baseDamage) * data.damageAmp;
    attribute.Damage(static_cast<int>(totalDamage));

    // Set Radius directly
    attribute.DamageRadius(static_cast<int>(data.radius));

    // Set Charges
    AttackCharges(data.charges);
}

void Projectile::Initialize(Transform cannonTrans, int _activeId, float cannonAngle, float shotPower) {
    isActive = true;
    id = _activeId;

    // Set projectile spawn pos at the front of the cannon muzzle
    // using cos and sin to calculate the direction of where the cannon is pointing at
    transform.pos.x = cannonTrans.pos.x + (cos(cannonAngle) * cannonTrans.size.x);
    transform.pos.y = cannonTrans.pos.y + (sin(cannonAngle) * cannonTrans.size.y);

    // Convert power and angle to 2d vector
    velocity.x = cos(cannonAngle) * shotPower;
    velocity.y = sin(cannonAngle) * shotPower;
    timer = Config::PROJECTILE_LIFESPAN;
}

void Projectile::ProjectileMovement(float dt) {
    // Grounded Check every frame
    isGrounded = PhysicsManager::IsObjectGrounded(transform);

    // Movement
    if (isGrounded) {
        velocity.x *= pow(PhysicsManager::FRICTION, dt);
        velocity.y = 0; // ensure that the projectile stay on the ground
        if (std::abs(velocity.x) < PhysicsManager::MINIMUM_VELOCITY) velocity.x = 0;
    }
    else {
        velocity.y -= PhysicsManager::GRAVITY * dt;
    }

    transform.pos.x += velocity.x * dt;
    transform.pos.y += velocity.y * dt;
}

void Projectile::OnCollision(Tile const& tile) {
    if (Mode() == ProjectileMode::PENTA_BURST && !isAbilityTriggered) return;

    if(CollisionManager::IsHorizontalCollision(transform, tile.transform)) {
        PhysicsManager::ResolveHorizontalCollision(transform, velocity, tile.transform);
    }
    else {
        PhysicsManager::ResolveVerticalCollision(transform, velocity, isGrounded, tile.transform);
    }

    if (!hasHitSomething && tile.IsDestructible()) {
        hasHitSomething = true;
    }
}

void Projectile::FirstTimeCollision() {
    float percentage = 0.5f;
    velocity.x *= percentage;
    velocity.y *= percentage;
    isCollisionTriggered = true;

    switch (Mode()) {
    case ProjectileMode::NUKE:
        EffectsManager::effectsController.PlayEffects(EffectsManager::Effects::NUKE, transform.pos);
    }
}

void Projectile::ConsumeAttackCharge() {
    if (--attackCharges <= 0) {
        attackCharges = 0;
        mode = ProjectileMode::IDLE;
    }
}

void Projectile::SetDestroy() {
    float speedSq = (velocity.x * velocity.x) + (velocity.y * velocity.y);

    // Start timer if it going to be destroy and projectile stops moving
    if (toBeDestroyed) {
        timer -= 1.0f * PhysicsManager::g_dt;
    }
    else { 
        // Check whatever or not projectile is set to be destroyed
        // if it grounded or speedSq is very less destroyed the projectile after a timer
        if (isGrounded && speedSq <= 0.1f) {
            timer = Config::PROJECTILE_LIFESPAN;
            toBeDestroyed = true;
        }
    }
}

bool Projectile::Destroy() {
    SetDestroy();
    // Destroy projectile
    if (timer <= 0.0f || !IsInMapBounds()) {
        isActive = isCollisionTriggered = toBeDestroyed = false;
        timer = 0.0f;
        return true; // projectile has been destroy
    }
    return false;
}

void Projectile::DamageTile(int tileX, int tileY) {
    Tile& centerTile = (*MapManager::currentMap)[tileY][tileX];
    if (Mode() == ProjectileMode::IDLE || !centerTile.IsDestructible()) return;

    int blastRadius = attribute.DamageRadius();

    std::cout << "===================== [Projectile] Damaging tiles =====================\n[Projectile] Damage Radius : " 
        + std::to_string(blastRadius) + " Center Tile " + GetTileName(centerTile.Type()) + " " + std::to_string(tileX) + " " + std::to_string(tileY)
        + "\n[Projectile] Remaining Attack Charges: " + std::to_string(AttackCharges()) << '\n';

    int minDX = tileX, minDY = tileY, maxDX = tileX, maxDY = tileY;
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

            if (!tileToDamage.IsDestructible()) continue;

            // Damage tile AFTER it has damage a CHARACTER compononent
            std::cout << "[Projectile] Damage Tile: " << tileToDamage << " Current hp: " << tileToDamage.attribute.Health() << " Damage: " << projDamage << '\n';
            tileToDamage.TakeDamage(projDamage);
            hasDamageAnyTile = true;
        }
    }

    // Consume 1 attack charge if projectile has damaged a tile 
    if (hasDamageAnyTile) {
        ConsumeAttackCharge();

        // Damage effect
        Tile& tileToDamage = (*MapManager::currentMap)[tileY][tileX];

        GraphicsManager::DrawGameObject(
            AssetsManager::circleTexture,
            GraphicsManager::CreateTransformMatrix(tileToDamage.transform.pos.x, tileToDamage.transform.pos.y, 0,
                (tileToDamage.transform.size.x * 0.5f) * blastRadius, (tileToDamage.transform.size.y * 0.5f) * blastRadius),
            { 0.75,0.0f,0 }
        );
    }
}

std::string Projectile::DebugInfo() {
    return "\n\nActive Projectile " + std::to_string(static_cast<int>(Mode())) + "\nSize: x: "
        + Utils::FormatFloat(transform.size.x, 2) + " y: " + Utils::FormatFloat(transform.size.y, 2)
        + "\nPos x: " + Utils::FormatFloat(transform.pos.x, 2) + " y " + Utils::FormatFloat(transform.pos.y, 2)
        + "\nVelocity x: " + Utils::FormatFloat(velocity.x, 2) + " y: " + Utils::FormatFloat(velocity.y, 2)
        + "\nGrounded: " + static_cast<std::string>(isGrounded ? "true" : "false")
        + "\nTo Be Destroyed: " + static_cast<std::string>(toBeDestroyed ? "true" : "false")
        + "\nTimer: " + Utils::FormatFloat(timer, 2)
        + "\nAttack Charges: " + std::to_string(AttackCharges())
        + "\nDamage Radius: " + std::to_string(attribute.DamageRadius())
        + "\nCollisionTriggered: " + static_cast<std::string>(IsCollisionTriggered() ? "true" : "false");
}