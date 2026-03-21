#include "Projectile.hpp"

#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "LevelManager.hpp"
#include "Character.hpp"
#include "CollisionManager.hpp"
#include "PhysicsManager.hpp"
#include "Utils.hpp"
#include "AbilitiesManager.hpp"
#include "AudioManager.hpp"
#include "EffectsManager.hpp"

AEVec2 defaultSize{ 10.0f , 10.0f };

Projectile::Projectile(AEGfxTexture* _texture, ProjectileMode _mode, AEVec2 _pos, int _attackCharges)
    : GameObject(_pos, defaultSize, _texture), mode(_mode), attackCharges{_attackCharges} {
}

void Projectile::Draw() {
    AEMtx33 mat = GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, transform.size.x, transform.size.y);
    GraphicsManager::DrawGameObject(texture, mat, Color{ 0.5f, 0.5f, 0, 1 });
}

bool Projectile::InIgnoredTiles() {
    if (ignoredTiles.size() <= 0) return false; // if there is no tiles to ignored return false

    // Current tileIndex based on current projectile transform
    TileIndex current{ 0,0 };
    Utils::GetTileIndex(current.x, current.y, transform.pos);

    // Check if current tile index is a tileIndex that should be ignored
    for (TileIndex const& ignoredTileId : ignoredTiles) {
        if (current == ignoredTileId) {
            std::cout << "In ignored Tiles" << '\n';
            return true;
        }
    }

    return false;
}

void Projectile::AbilityActivation(bool isPlayer, Ability const& ability) {
    if (mode == ProjectileMode::ATTACK || isAbilityTriggered) return;

    bool leftClick = AEInputCheckTriggered(AEVK_LBUTTON);

    switch (Mode()) {
    case ProjectileMode::BOMB:
        if (leftClick) {
            isAbilityTriggered = true;
            // Bomb instant explode and damage tile blocks double its radius once activated
            int tileX = 0, tileY = 0;
            Utils::GetTileIndex(tileX, tileY, transform.pos);
            int radius = attribute.DamageRadius();

            // Increased radius damage
            if (!Utils::IsTileIndexOutOfBounds(tileX, tileY)) {
                attribute.DamageRadius(ability.GetStatus().effectRadius);
                DamageTile(tileX, tileY);
                InstantDestroy();
            }

            // Reset stats back to default
            attribute.DamageRadius(radius);
            PlayModeEffect();
            std::cout << "BOMB mode triggered\n";
        }
        break;
    case ProjectileMode::NUKE:
        if (leftClick) {
            isAbilityTriggered = true;
            velocity = { 0,0 }; // fall straight down immediately
            std::cout << "NUKE mode triggered\n";
        }
        break;
    case ProjectileMode::PENTA_BURST:
        if (hasCollided) { // Give a velocity boost once only to continue hitting more stuff
            isAbilityTriggered = true;
            velocity.x *= 2;
            velocity.y *= 2;
        }
        break;
    case ProjectileMode::WALL:
        if (leftClick) {
            isAbilityTriggered = true;

            // Create a 1 tile width wall, 1 tiles x index behind current projectile x index
            int tileX = 0, tileY = 0;
            Utils::GetTileIndex(tileX, tileY, transform.pos);
            tileX -= 1; 

            if (!Utils::IsTileIndexOutOfBounds(tileX, tileY)) {
                for (int i = 0; i <= Config::MAX_GRID_Y; ++i) {
                    Tile& tile = (*MapManager::currentMap)[i][tileX];
                    if (tile.Type() == TileType::TILE_EMPTY) {
                        tile.Type(isPlayer ? TileType::TILE_BLOCK_1 : TileType::TILE_BLOCK_2);
                        tile.attribute.Health(ability.GetStats().hp);
                    }
                }
            }
            std::cout << "WALL mode triggered\n";
        }
        break;
    case ProjectileMode::PIERCING:
        canBypassBlocks = isAbilityTriggered = true;
        break;
    case ProjectileMode::DECAY_SHOT:
        if (hasCollided) {
            isAbilityTriggered = true;
            SetAreaEffects(ability, true);
        }
        break;
    case ProjectileMode::POISON:
        if (hasCollided) {
            isAbilityTriggered = true;
            SetAreaEffects(ability, false);
            std::cout << "Poison!" << '\n';
        }
        break;
    case ProjectileMode::INSTANT_BASTION:
        if (leftClick) {
            isAbilityTriggered = true;

            TileIndex center{ 0,0 }, min{ 0,0 }, max{ 0,0 };
            Utils::GetTileIndex(center.x, center.y, transform.pos); // center index
            center.x -= ability.GetStatus().effectRadius;
            Utils::CalculateTileBounds(center, min, max, ability.GetStatus().effectRadius);

            if (!Utils::IsTileIndexOutOfBounds(center.x, center.y)) {
                for (int y = max.y; y < min.y; ++y) {
                    for (int x = min.x; x < max.x; ++x) {
                        Tile& tile = (*MapManager::currentMap)[y][x];
                        if (tile.Type() == TileType::TILE_EMPTY) {
                            tile.Type(isPlayer ? TileType::TILE_BLOCK_1 : TileType::TILE_BLOCK_2);
                            tile.attribute.Heal(1.0f); // heal tile to full hp incase
                        }

                        ignoredTiles.push_back({ x,y }); // ignore these newly builded blocks for a turn
                    }
                }
            }
        }
        break;
    }
}

void Projectile::SetProjectileStats(std::string _projectileName, ProjectileModule const& data) {
    projectileName = _projectileName;
    projectileId = data.projectileId;
    mode = static_cast<ProjectileMode>(data.projectileId);

    // Total Damage = Base * Multiplier
    float totalDamage = static_cast<float>(data.baseDamage) * data.damageMultiplier;
    attribute.Damage(static_cast<int>(totalDamage));

    // Set Radius directly
    attribute.DamageRadius(static_cast<int>(data.radius));

    // Set Charges
    AttackCharges(data.charges);
}

void Projectile::Initialize(Transform const& cannonTransform, int _activeId, float cannonAngle, float shotPower) {
    // Set projectile spawn pos at the front of the cannon muzzle
    // using cos and sin to calculate the direction of where the cannon is pointing at
    Transform const& trans = {
        cannonTransform.pos.x + (cos(cannonAngle) * cannonTransform.size.x),
        cannonTransform.pos.y + (sin(cannonAngle) * cannonTransform.size.y),
    };

    // Convert power and angle to 2d vector velocity
    AEVec2 const& _velocity = {
        cos(cannonAngle) * shotPower,
        sin(cannonAngle) * shotPower,
    };

    Initialize(trans, _activeId, _velocity);
}

void Projectile::Initialize(Transform const& _transform, int _activeId, AEVec2 const& _velocity) {
    isActive = true;
    id = _activeId;

    // Set spawn position
    transform.pos.x = _transform.pos.x;
    transform.pos.y = _transform.pos.y;

    // Set velocity
    velocity.x = _velocity.x;
    velocity.y = _velocity.y;
    timer = Config::PROJECTILE_LIFESPAN;
}

void Projectile::Update(bool isPlayerShot, float dt) {
    AEVec2 start = transform.pos;
    // Set movement first
    ProjectileMovement(dt);
    AEVec2 end = transform.pos;

    if (InIgnoredTiles()) return; // if is currently inside a tile block to ignored just return no collision checks
    if (hasHitShield) return; // if projectile has hit a shield it should get destroy instantly no more collision checks

    // Collision Checks

    // if its player shot get the AI shield to check shield collision
    TowerShield& shield = LevelManager::CurrentLevel()->GetTowerShield(!isPlayerShot);
    if (shield.IsShieldOn()) {
        hasHitShield = CollisionManager::CheckContinuousCollision(start, end, shield.transform);
    }

    if (hasHitShield) {
        if (Mode() == ProjectileMode::SHIELD_BUSTER) {
            shield.TakeDamage(0, true);
            PlayModeEffect();
        }
        else {
            shield.TakeDamage(attribute.Damage());
        }

        InstantDestroy();
        return;
    }

    // Check for collision in a 3x3 area with the projectile

    int tileX = 0, tileY = 0;
    Utils::GetTileIndex(tileX, tileY, transform.pos);

    int minX = tileX - 1, minY = tileY - 1, maxX = tileX + 1, maxY = tileY + 1;
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            if (Utils::IsTileIndexOutOfBounds(x, y)) continue;

            Tile& tile = (*MapManager::currentMap)[y][x];

            if (tile.IsEmpty()) continue; // If it hits a empty tile continue does not need to check a empty tile

            bool hasTileCollision = CollisionManager::AreCircleSquareIntersecting(transform, tile.transform);

            // Checking collision of the projectile with the tile blocks that is collidable
            if (hasTileCollision) {
                // Damage tile first, incase of destroying the blocks and bouncing back if damageTile is not called first
                DamageTile(x, y);
                if (!canBypassBlocks && !hasCollided) {
                    FirstTimeCollision();
                }

                if (!canBypassBlocks) {
                    ConstantCollision(tile);
                }

                //// After bypassing a block/character immediately disable by pass wall
                //if (canBypassBlocks) {
                //    canBypassBlocks = false;
                //}
            }
        }
    }
}

void Projectile::ProjectileMovement(float dt) {
    // Grounded Check every frame
    if (!canBypassBlocks) {
        isGrounded = PhysicsManager::IsObjectGrounded(transform);
    }

    if (!InIgnoredTiles()) {
        if (isGrounded) {
            velocity.x *= pow(PhysicsManager::FRICTION, dt);
            velocity.y = 0; // ensure that the projectile stay on the ground
            if (std::abs(velocity.x) < PhysicsManager::MINIMUM_VELOCITY) velocity.x = 0;
        }
        else {
            // Gravity pull
            velocity.y -= (PhysicsManager::GRAVITY * gravityScale) * dt;
        }
    }

    // Movement
    transform.pos.x += velocity.x * dt;
    transform.pos.y += velocity.y * dt;
}

void Projectile::ConstantCollision(Tile const& tile) {
    if (Mode() == ProjectileMode::PENTA_BURST && !isAbilityTriggered) return;

    if(CollisionManager::IsHorizontalCollision(transform, tile.transform)) {
        PhysicsManager::ResolveHorizontalCollision(transform, velocity, tile.transform);
    }
    else {
        PhysicsManager::ResolveVerticalCollision(transform, velocity, isGrounded, tile.transform);
    }

    if (!hasHit && tile.IsDestructible()) {
        hasHit = true;
    }
}

void Projectile::FirstTimeCollision() {
    // Reduce velocity when it hit something the first time
    float percentage = 0.5f;
    velocity.x *= percentage;
    velocity.y *= percentage;
    hasCollided = true;

    // Play audio
    AudioManager::PlaySFX(AudioManager::SFX::Impact);

    // Set effects
    PlayModeEffect();
}

void Projectile::ConsumeAttackCharge() {
    if (--attackCharges <= 0) {
        attackCharges = 0;
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
        // if it grounded or speedSq is very small, destroyed the projectile after a timer
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
        InstantDestroy();
        return true; // projectile has been destroy
    }
    return false;
}

void Projectile::InstantDestroy() {
    isActive = hasCollided = toBeDestroyed = false;
    timer = 0.0f;
}

void Projectile::PlayModeEffect() {
    EffectsManager::effectsController.PlayProjectiileAbiltiesEffect(static_cast<int>(Mode()), transform.pos);
}

void Projectile::DamageTile(int tileX, int tileY) {
    Tile& centerTile = (*MapManager::currentMap)[tileY][tileX];

    if (attackCharges <= 0 /*|| !centerTile.IsDestructible()*/) return; // Can't damage return

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
            if (Utils::IsTileIndexOutOfBounds(dX, dY)) continue;

            Tile& tileToDamage = (*MapManager::currentMap)[dY][dX];
            //std::cout << "[Projectile] Checked tile " << tileToDamage << '\n';

            if (!tileToDamage.IsDestructible()) continue;

            // Damage a tile and character is also damage inside the TILE class TakeDamage
            std::cout << "[Projectile] Damage Tile: " << tileToDamage << " Current hp: " << tileToDamage.attribute.Health() << " Damage: " << projDamage << '\n';
            tileToDamage.TakeDamage(projDamage);
            
            if (mode == ProjectileMode::SHIELD_BUSTER && tileToDamage.attribute.IsShieldActive()) {
                tileToDamage.attribute.DamageShield(0.0f);
            }

            hasDamageAnyTile = true;

            LevelManager::CurrentLevel()->RecordDamageLog(projDamage, tileToDamage.IsCharacter());
        }
    }

    // Consume 1 attack charge if projectile has damaged a tile 
    if (hasDamageAnyTile) {
        ConsumeAttackCharge();
    }
}

void Projectile::SetAreaEffects(Ability const& ability, bool isDecaying) {
    Level* currentLevel = LevelManager::CurrentLevel();
    int radius = ability.GetStatus().effectRadius;

    TileIndex center{ 0,0 }, min{ 0,0 }, max{ 0,0 };
    Utils::GetTileIndex(center.x, center.y, transform.pos); // set current projectile tile index as center
    Utils::CalculateTileBounds(center, min, max, radius); // calculate the bounds to set the blocks inside the bounds to be decaying

    // Check tiles within a given radius
    for (int y = max.y; y < min.y; ++y) {
        for (int x = min.x; x < max.x; ++x) {
            Tile& tile = (*MapManager::currentMap)[y][x];

            if (isDecaying) { // decay blocks within a tile radius
                if (!tile.IsBlocks()) continue;

                tile.attribute.SetTickDamage(ability.GetStatus().damageEachRound, ability.GetStatus().rounds); // set decay or to poison
                tile.UpdateTextureColor(1); // update texture color of the tiles after it has been set to decay
                currentLevel->AddDecayTiles(tile.tileId); // add to the vector containing all the decaying tiles
            }
            else { // poison character within a tile radius
                if (!tile.IsCharacter()) continue;
                // Find character to poison
                Character* characterToPoison = currentLevel->FindCharacterObj(tile);
                characterToPoison->attribute.SetTickDamage(ability.GetStatus().damageEachRound, ability.GetStatus().rounds);
                characterToPoison->UpdateTextureColor(1);
                currentLevel->AddPoisonCharacters(characterToPoison->IsPlayer(), characterToPoison->id); // add to the vector containing all the poison tiles
            }
        }
    }
}

std::string Projectile::DebugInfo() {
    return "\n\nActive Projectile " + std::to_string(static_cast<int>(Mode())) + "\nSize: x: "
        + Utils::FormatFloat(transform.size.x, 2) + " y: " + Utils::FormatFloat(transform.size.y, 2)
        + "\nPos x: " + Utils::FormatFloat(transform.pos.x, 2) + " y " + Utils::FormatFloat(transform.pos.y, 2)
        + "\nVelocity x: " + Utils::FormatFloat(velocity.x, 2) + " y: " + Utils::FormatFloat(velocity.y, 2)
        + "\nGrounded: " + std::string(isGrounded ? "true" : "false")
        + "\nTo Be Destroyed: " + std::string(toBeDestroyed ? "true" : "false")
        + "\nTimer: " + Utils::FormatFloat(timer, 2)
        + "\nAttack Charges: " + std::to_string(AttackCharges())
        + "\nDamage Radius: " + std::to_string(attribute.DamageRadius())
        + "\nCollisionTriggered: " + std::string(hasCollided ? "true" : "false")
        + "\nCanBypassWall: " + std::string(canBypassBlocks ? "true" : "false");
}