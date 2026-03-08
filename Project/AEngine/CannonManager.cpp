#include "CannonManager.hpp"

#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "CameraManager.hpp"
#include "GameStateManager.hpp"
#include "Utils.hpp"
#include "CollisionManager.hpp"
#include "Tile.hpp"
#include "AbilitiesManager.hpp"
#include "UIManager.hpp"
#include "LevelManager.hpp"
#include "PhysicsManager.hpp"

namespace CannonManager {
    Cannon::Cannon(AEVec2 _pos, AEGfxTexture* _texture, TileIndex _tileId, AEVec2 _size, bool _isShootingRight, bool _isPlayerControlled)
        : GameObject(_pos, _size, _texture, _tileId, Color{ 1,1,1,1 }), isShootingRight{ _isShootingRight }, isPlayerControlled{ _isPlayerControlled } {
    }

    void Cannon::SetAbilityController(AbilitiesManager::AbilityController* controller) {
        abilityController = controller;
        controller->ChangeAbility(0); // Set default projectile as the default ability
        ProjectileSettings();
    }
    
    void Cannon::ProjectileSettings() {
        // Reset to default
        activeProjectiles.clear(); // ensure that there should not be any active ammo when cannon is reset
        projectiles.clear();
        projectiles.resize(projectileCount);

        std::vector<Ability>& allProjectileAbilities = abilityController->GetOffensiveAbilities();
        projectileAbilitiesCount = static_cast<int>(allProjectileAbilities.size());

        // Initializing the default and ALL projectiles with abilities
        if (projectiles.empty()) std::cerr << "[CannonManager] Cannon Projectiles vector is EMPTY" << '\n';

        for (int i = 0; i < projectiles.size(); ++i) {
            // Assign default projectile ability if the the projectils size is larger than the allProjectileAbilities size
            int abilityId = 0;
            if (i < allProjectileAbilities.size()) {
                abilityId = i;
            }

            // Assign ability stats to the projectiles
            Ability const& ability = allProjectileAbilities[abilityId];
            Projectile& projectile = projectiles[i];

            // Initialize projectile with its texture
            projectile = { AssetsManager::circleTexture };
            projectile.SetProjectileStats(ability.GetName(), ability.GetProj());

            //std::cout << "[CannonManager] Set " << ability.Id() << " Mode: " << projectile.ModeName() << " " << ability.GetName() << " ability into the cannon projectile vector" << '\n';
        }
    }
    
    void Cannon::Update() {
        if (isPlayerControlled) {
            float worldMouseX = 0, worldMouseY = 0;
            Utils::GetWorldMousePos(worldMouseX, worldMouseY);

            // Calculate angle toward mouse in world space
            cannonAngle = atan2(worldMouseY - transform.pos.y, worldMouseX - transform.pos.x);

            //std::cout << "Angle: " << cannonAngle << "Mouse x: " << worldMouseX << " y:" << worldMouseY
            //    << " Cannon pos x: " << transform.pos.x << " y: " << transform.pos.y << '\n';
            // Add limits so it can only turn facing upwards or downwards limits
            ClampAngle(cannonAngle);
            DrawTrajectory();
        }
        else {
            EnemyAI();
        }

        SelectProjectileAbility();
        ChargeShot();
        UpdateProjectile();
        Debugger();
    }

    void Cannon::Draw() {
        // Render cannon
        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, cannonAngle, transform.size.x, transform.size.y);
        GraphicsManager::DrawGameObject(texture, mat, textureColor);
    }

    void Cannon::DrawUI() {
        if (hasCannonShot) return;
        // Render UI
        if (isHoldingLeftClick) {
            float x = transform.pos.x - (transform.size.x * 1.5f);
            if (!isShootingRight) x = transform.pos.x + (transform.size.x * 1.5f);
            UIManager::Bar bar{ { x, transform.pos.y + (transform.size.y * 1.5f) },
                { 15,100 }, Color{ 0,1,0 }, UIManager::FillType::TOP_TO_BOTTOM };
            bar.Draw(chargingPower, maxPower, false);

            if (!isPlayerControlled) return;

            Transform trans{ { 0,0 }, {500,25} };
            Utils::SetAnchoredPosition(trans, Utils::AnchorMode::BOTTOM_CENTER);

            UIManager::Bar uiBar{ trans.pos,
                trans.size, Color{ 0,1,0 }, UIManager::FillType::LEFT_TO_RIGHT };
            uiBar.Draw(chargingPower, maxPower);
        }
    }

    void Cannon::ResetTurn() {
        isProjectileDestroyed = false;
        hasCannonShot = false;
        SetCamera(false);

        // Reselect AI ability
        if (!isPlayerControlled && abilityController) {
            abilityController->ResetAIAbilitySelection();
        }
    }

    void Cannon::SetTarget(Character* _target) {
        target = _target;
        std::cout << "[CannonManager] Target set" << '\n';
    }

    bool Cannon::IsTargetAlive() {
        if (!target) return false;

        // Remove target ptr if current target has been destroyed
        if (!target->attribute.IsActive()) {
            target = nullptr;
            return false;
        }

        return true;
    }

    void Cannon::SetEnemyAI(int difficultyLevel) {
        switch (difficultyLevel) {
        default:
        case 0: // Easy
            aiDifficulty = 0.9f;
            break;
        case 1: // Medium
            aiDifficulty = 0.95f;
            break;
        case 2: // Hard
            aiDifficulty = 0.99f;
            break;
        }
    }

    void Cannon::EnemyAI() {
        if (hasCannonShot || isProjectileDestroyed) return;
        if (!target) {
            std::cerr << "[CannonManager] Can't find any player character targets for the enemy to shoot!" << '\n';
            return;
        }
        if (!abilityController) {
            std::cerr << "[CannonManager] EnemyAI failed. abilityController is nullptr\n";
            return;
        }

        if (!canAIShoot) {
            abilityController->AIAbilitySelection();
            EnemyAICalculation(target->transform.pos);

            // Set shoot timer to add a delay between the start of the enemy turn and enemy shooting the projectile out
            float minTimer = 1.5f;
            float maxTimer = 3.0f;

            aiShootTimer = Utils::RandomValue(minTimer, maxTimer);
            canAIShoot = true;
        }
        else {
            aiShootTimer -= PhysicsManager::g_dt;

            if (aiShootTimer <= 0.0f) {
                ShootProjectile();
                canAIShoot = false;
            }
        }
    }

    void Cannon::EnemyAICalculation(AEVec2 const targetPos) {
        float dx = targetPos.x - transform.pos.x;
        float dy = targetPos.y - transform.pos.y;
        float distance = sqrt(dx * dx + dy * dy);

        // Randomise angle between the min and max radian
        float min = 30 * (PI / 180.0f);
        float max = 60 * (PI / 180.0f);
        float randomAngleBase = Utils::RandomValue(min, max);

        // Adjust for direction (Shooting left or right)
        float launchAngle = (dx > 0) ? randomAngleBase : (PI - randomAngleBase);

        // Calculate required Power (Initial Velocity)
        // Basic range formula: v = sqrt( (g * d) / sin(2 * theta) )
        float neededVelocity = sqrt((PhysicsManager::GRAVITY * abs(distance)) / sin(2 * randomAngleBase));

        // Randomise offset to add a variance to 
        float inaccuracy = (1.0f - aiDifficulty);
        float randomOffset = Utils::RandomValue(-inaccuracy, inaccuracy);

        // Apply the variables to the cannon
        cannonAngle = launchAngle;
        shotPower = std::clamp(neededVelocity * (1.0f + randomOffset), minPower, maxPower);

        std::cout << "======================= [CannonManager] Enemy AI Shot ========================\n"
            << "[CannonManager] Enemy AI Target pos x: " << targetPos.x << " y: " << targetPos.y << '\n'
            << "[CannonManager] Enemy AI Ideal: angle: " << launchAngle << " shot power: " << neededVelocity << '\n'
            << "[CannonManager] Enemy AI Actual: angle: " << cannonAngle << " shot power: " << shotPower << '\n';
    }

    void Cannon::ChargeShot() {
        if (hasCannonShot == true) return;

        // Shoot when the button is RELEASED
        if (isPlayerControlled && AEInputCheckReleased(AEVK_LBUTTON)) {
            if (isHoldingLeftClick) {
                shotPower = chargingPower;
                ShootProjectile();
            }

            // Reset for next shot
            chargingPower = minPower;
            canPlayerShoot = true;
        }

        // Cancel player shooting if right mouse click is input
        if (isHoldingLeftClick && AEInputCheckTriggered(AEVK_RBUTTON)) {
            canPlayerShoot = isHoldingLeftClick = false;
        }

        if (isPlayerControlled && !canPlayerShoot) return;

        if (AEInputCheckCurr(AEVK_LBUTTON) || canAIShoot == true) {
            isHoldingLeftClick = canPlayerShoot = true;

            float chargeRateWithDT = chargeRate * PhysicsManager::g_dt;
            chargingPower += isPowerIncreasing ? chargeRateWithDT : -chargeRateWithDT;

            if (chargingPower > maxPower) {
                chargingPower = maxPower;
                isPowerIncreasing = false;
            }
            if (chargingPower < minPower) {
                chargingPower = minPower;
                isPowerIncreasing = true;
            }

            // When player press left mouse click, set camera to position the cannon and unable to move the camera
            if (resetCamWhenShooting) {
                CameraManager::camera.SetPosition(transform.pos);
                CameraManager::camera.SetState(CameraManager::CameraController::CameraState::STATIC);
            }
        }

        if (AEInputCheckReleased(AEVK_LBUTTON)) {
            isHoldingLeftClick = false;
        }
    }

    void Cannon::ClampAngle(float& angle) {
        if (isShootingRight) {
            angle = std::clamp(angle, -1.5f, 1.5f);
        }
        else {
            // Clamp to left hemisphere
            if (angle > 0) {
                angle = std::clamp(angle, 1.5f, 3.5f);   // Upper left: 1.5 to 3.5
            }
            else {
                angle = std::clamp(angle, -3.5f, -1.5f); // Lower left: -3.5 to -1.5
            }
        }
    }

    void Cannon::SelectProjectileAbility() {
        if (!abilityController) {
            std::cerr << "[CannonManager] Can't find any abilityController" << '\n';
            return;
        }

        selectedProjectileId = abilityController->FindProjectileId();
    }

    void Cannon::ShootProjectile() {
        if (hasCannonShot) return;

        // IMPORTANT: use a COPY here so we don't permanently modify the template projectile inside projectiles[].
        Projectile projectile = projectiles[selectedProjectileId];
        projectile.isActive = true;
        projectile.id = static_cast<int>(activeProjectiles.size());

        projectile.transform.pos.x = transform.pos.x + (cos(cannonAngle) * transform.size.x);
        projectile.transform.pos.y = transform.pos.y + (sin(cannonAngle) * transform.size.y);

        projectile.velocity.x = cos(cannonAngle) * shotPower;
        projectile.velocity.y = sin(cannonAngle) * shotPower;
        projectile.timer = Config::PROJECTILE_LIFESPAN;

        // Add the these projectile to the active projectile list
        activeProjectiles.push_back(projectile);
        activeProjectiles.back().id = static_cast<int>(activeProjectiles.size() - 1);

        // Set projectile ability effects here
        abilityController->UseAbility();

        SetCamera(true); // Follow the projectile that is shot out of the cannon
        hasCannonShot = true;
        std::cout << "[CannonManager] Projectile Fired!\n";
    }

    void Cannon::UpdateProjectile() {
        if (activeProjectiles.empty()) return;

        std::vector<Projectile*> projPtrToDestroy;

        for (Projectile& projectile : activeProjectiles) {
            if (!projectile.isActive) continue;

            projectile.AbilityUpdate();

            float dt = PhysicsManager::g_dt;
            // Increased the frequency of the collision check by using loops
            // ensuring faster moving projectiles are check for collision multiple times
            // to ensure that the GHOSTING of block bug does not happen
            // Using sub_steps to ensure that it still moving at a per frame rate time (dt or 1/10 dt but 10 loops) even though it is looping
            const int SUB_STEPS = 10;
            float subDt = dt / SUB_STEPS;

            for (int step = 0; step < SUB_STEPS; ++step) {
                // Set movement first
                projectile.MoveProjectile(subDt);
                projectile.isGrounded = false;

                // Check for collision in a 3x3 area with the projectile
                int gridX = 0, gridY = 0;
                Utils::GetGridIndex(gridX, gridY, projectile.transform.pos);

                int minX = gridX - 1, minY = gridY - 1, maxX = gridX + 1, maxY = gridY + 1;
                for (int y = minY; y <= maxY; ++y) {
                    for (int x = minX; x <= maxX; ++x) {
                        if (Utils::IsGridIndexOutOfBounds(x, y)) continue;

                        Tile& tile = (*MapManager::currentMap)[y][x];

                        if (tile.IsEmpty()) continue; // If it hits a empty tile continue

                        bool hasCollision = CollisionManager::AreCircleSquareIntersecting(projectile.transform, tile.transform);

                        // Checking collision of the projectile with the tile blocks that is collidable
                        if(hasCollision) {
                            // Damage tile first, incase of destroying the blocks and bouncing back if damageTile is not called first
                            projectile.DamageTile(x, y);

                            if (!projectile.collisionTriggered) {
                                // If the projectile hit something, REDUCE the velocity by a percentage for the FIRST COLLISION ONLY
                                // So that it does not continue having the same projectile velocity even if it hits and destroy something
                                float percentage = 0.5f;
                                projectile.velocity.x *= percentage;
                                projectile.velocity.y *= percentage;
                                projectile.collisionTriggered = true;
                            }
                            else {
                                projectile.OnCollision(tile);
                            }
                        }
                    }
                }

                // SECONDARY CHECK IF THE PROJECTILE IS GROUNDED OR NOT
                if (!projectile.isGrounded) {
                    // Only and MUST check 2x3 for additonal checks
                    for (int y = gridY; y <= maxY; ++y) {
                        for (int x = minX; x <= maxX; ++x) {
                            if (Utils::IsGridIndexOutOfBounds(x, y)) continue;

                            Tile& tile = (*MapManager::currentMap)[y][x];

                            if (!tile.IsEmpty()) {
                                projectile.isGrounded = PhysicsManager::IsGrounded(projectile.transform, tile.transform);
                                if (projectile.isGrounded) {
                                    projectile.velocity.y = 0;
                                    break;
                                }
                            }
                        }
                        //if (projectile.isGrounded) break;
                    }
                }
            }

            // Add to projectile to be destroyed list if true
            if (DestroyProjectile(projectile)) {
                projPtrToDestroy.push_back(&projectile);
            }
            projectile.Render();
        }

        //Remove projectiles from the active projectiles list return if vector to destroy is empty
        if (projPtrToDestroy.empty()) return;

        for (Projectile* projToDestroy : projPtrToDestroy) {
            // Find the index containing the projectile and erase from the active ammo vector
            for (int i = 0; i < activeProjectiles.size(); i++) {
                if (activeProjectiles[i].id == projToDestroy->id) {
                    activeProjectiles.erase(activeProjectiles.begin() + i);
                    //SetCamera(false); // DISABLE to not allow camera to move back to the cannon after projectile is destroyed
                    std::cout << "[CannonManager] Remove projectile from vector. Active vector count: " << activeProjectiles.size() << '\n';
                }
            }
        }
    }

    bool Cannon::DestroyProjectile(Projectile & projectile) {
        float speedSq = (projectile.velocity.x * projectile.velocity.x) + (projectile.velocity.y * projectile.velocity.y);

        // Start timer if it going to be destroy and projectile stops moving
        if (projectile.toBeDestroyed) {
            projectile.timer -= 1.0f * PhysicsManager::g_dt;
        }
        else {
            if (projectile.isGrounded && speedSq <= 0.1f) {
                projectile.timer = Config::PROJECTILE_LIFESPAN;
                projectile.toBeDestroyed = true;
            }
        }

        // Destroy projectile
        if (projectile.timer <= 0.0f || !projectile.IsInMapBounds()) {
            projectile.Destroy();
            //hasCannonShot = false; // reset cannon back to false to allow player to shoot again after projectile been destroyed
            isProjectileDestroyed = true;
            return true;
        }
        return false;
    }

    void Cannon::SetCamera(bool isFollowingProjectile) {
        if (isFollowingProjectile) {
            // Set camera to follow the newly fired projectile
            CameraManager::camera.SetFollowTarget(&activeProjectiles.back().transform.pos);
            CameraManager::camera.SetState(CameraManager::CameraController::CameraState::FOLLOW);
        }
        else {
            // Return camera to input control
            CameraManager::camera.SetFollowTarget(nullptr);
            CameraManager::camera.SetState(CameraManager::CameraController::CameraState::INPUT);
            CameraManager::camera.SetPosition(transform.pos);
            // TODO: RESET ZOOM back to default
        }
    }

    void Cannon::DrawTrajectory() {
        if (hasCannonShot) return; 
        int numDots = 30;
        // Starting position (at the barrel tip, matching ShootProjectile)
        float startX = transform.pos.x + cos(cannonAngle) * transform.size.x;
        float startY = transform.pos.y + sin(cannonAngle) * transform.size.x;

        // Initial velocity components
        float vx = cos(cannonAngle) * maxPower;
        float vy = sin(cannonAngle) * maxPower;

        // Time step between each dot (e.g., 0.1 seconds into the future)
        float timeStep = 0.05f;

        for (int i = 0; i < numDots; i++) {
            float t = i * timeStep;

            // Kinematic equations
            float drawX = startX + (vx * t);
            float drawY = startY + (vy * t) - (0.5f * PhysicsManager::GRAVITY * t * t);

            float alpha = 1.0f - static_cast<float>(i / numDots);
            GraphicsManager::DrawGameObject(AssetsManager::circleTexture, GraphicsManager::CreateTransformMatrix(drawX, drawY, 0, 10, 10), { 1,1,1, alpha });
        }
    }

    void Cannon::Debugger() {
        if (!Utils::isDebuggerOn) return;

        // Toggle through all the ability
        if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
            if (AEInputCheckTriggered(AEVK_T)) {
                abilityController->ToggleAbility(true);
            }
        }
        else {
            if (AEInputCheckTriggered(AEVK_T)) {
                abilityController->ToggleAbility(false);
            }
        }

        if (AEInputCheckTriggered(AEVK_Y)) {
            Utils::Wrap(++aiDifficulty, 0.0f, 1.0f);
            std::cout << "[CannonManager] AI Difficulty: " + std::to_string(aiDifficulty) << '\n';
        }

        float mX = 0, mY = 0;
        Utils::GetWorldMousePos(mX, mY);
        float cX = transform.pos.x, cY = transform.pos.y;
        Utils::GetWorldPos(cX, cY);
        s32 mouseX, mouseY;
        AEInputGetCursorPosition(&mouseX, &mouseY);

        Projectile const& selectedProjectile = projectiles[selectedProjectileId];
        Ability selectedAbility = abilityController->CurrentAbility();

        // Show cannon information
        std::string debugStr = "Cannon " + static_cast<std::string>(isPlayerControlled ? "PLAYER" : "ENEMY")
            + "\nIs Active : " + std::string(hasCannonShot ? "true" : "false")
            + "\nIs Projectile destroyed : " + static_cast<std::string>(isProjectileDestroyed ? "true" : "false")
            + "\nPos x: " + Utils::FormatFloat(transform.pos.x, 2) + " y: " + Utils::FormatFloat(transform.pos.y, 2)
            + "\nAngle: " + Utils::FormatFloat(cannonAngle * (180.0f / 3.14159265359), 2) + " " + Utils::FormatFloat(cannonAngle, 2)
            + "\n\nSelected Projectile\nId: " + std::to_string(selectedProjectile.ProjectileId())
            + "\nName: " + selectedProjectile.ProjectileName()
            + "\nMode: " + selectedProjectile.ModeName()
            + "\nDamage: " + std::to_string(selectedProjectile.attribute.Damage())
            + "\nRadius: " + std::to_string(selectedProjectile.attribute.DamageRadius())
            + "\n\nAbilities Inventory Count: " + std::to_string(abilityController->InventoryCount())
            + "\nSelected Ability:\nId:" + std::to_string(selectedAbility.Id())
            + "\nName: " + selectedAbility.GetName()
            + "\nAbility Charges Remaining: " + std::to_string(selectedAbility.AbilityCharges());
                
        if (!isPlayerControlled) {
            debugStr += "\nAI Shoot Timer: " + Utils::FormatFloat(aiShootTimer, 2);
        }
        else {
            debugStr += "\n\nShortcuts:\n[CTRL + T] Toggle ALL Abilities\n[T] Toggle Inventory Abilities";
        }

        // Print projectile that is shoot out information
        if (activeProjectiles.size() > 0) {
            Projectile const& projectile = activeProjectiles.back();
            debugStr += "\n\nActive Projectile " + std::to_string(static_cast<int>(projectile.Mode())) + "\nSize: x: "
                + Utils::FormatFloat(projectile.transform.size.x, 2) + " y: " + Utils::FormatFloat(projectile.transform.size.y, 2)
                + "\nPos x: " + Utils::FormatFloat(projectile.transform.pos.x, 2) + " y " + Utils::FormatFloat(projectile.transform.pos.y, 2)
                + "\nVelocity x: " + Utils::FormatFloat(activeProjectiles.back().velocity.x, 2) + " y: " + Utils::FormatFloat(projectile.velocity.y, 2)
                + "\nGrounded: " + static_cast<std::string>(projectile.isGrounded ? "true" : "false")
                + "\nTo Be Destroyed: " + static_cast<std::string>(projectile.toBeDestroyed ? "true" : "false")
                + "\nTimer: " + Utils::FormatFloat(projectile.timer, 2)
                + "\nAttack Charges: " + std::to_string(projectile.AttackCharges())
                + "\nDamage Radius: " + std::to_string(projectile.attribute.DamageRadius())
                + "\nCollisionTriggered: " + static_cast<std::string>(projectile.collisionTriggered ? "true" : "false");
        }

        TextObject textObj{ debugStr, Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, {0.5f, 0.99f} };
        textObj.Draw();
    }
}