#include "CannonManager.hpp"

#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Color.hpp"
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
        ProjectileSettings();
    }

    void Cannon::SetAbilityController(AbilitiesManager::AbilityController* controller) {
        abilityController = controller;
    }

    bool Cannon::HasShot() const { return hasShot; }

    void Cannon::ProjectileSettings() {
        // Reset to default
        activeProjectiles.clear(); // ensure that there should not be any active ammo when cannon is reset
        projectiles.clear();
        projectiles.resize(projectileCount);

        // Initalize all the projectile variables
        for (int i = 0; i < projectileCount; ++i) {
            Projectile& projectile = projectiles[i];
            projectile = { AssetsManager::circleTexture, static_cast<ProjectileMode>(i+1) };
            AbilitiesManager::SetProjectileAbility(projectile);
        }
    }

    void Cannon::Update() {
        if (isPlayerControlled) {
            float worldMouseX = 0, worldMouseY = 0;
            Utils::GetWorldMousePos(worldMouseX, worldMouseY);

            // Calculate angle toward mouse
            cannonAngle = atan2(worldMouseY - transform.pos.y, worldMouseX - transform.pos.x);

            // Add limits so it can only turn facing upwards or downwards limits
            ClampAngle(cannonAngle);
            DrawTrajectory();
            SelectProjectile();
        }
        ChargeShot();
        UpdateProjectile();

        // Debugger
        if (Utils::isDebuggerOn) {
            float mX = 0, mY = 0;
            Utils::GetWorldMousePos(mX, mY);
            float cX = transform.pos.x, cY = transform.pos.y;
            Utils::GetWorldPos(cX, cY);
            s32 mouseX, mouseY;
            AEInputGetCursorPosition(&mouseX, &mouseY);

            Projectile& selectedProjectile = projectiles[selectedProjectileId];

            std::string debugStr = "Cannon Is Player:" + std::string(isPlayerControlled ? "true" : "false")
                + "\nIs Active: " + std::string(isActive ? "true" : "false") + "\nHas Shot : " + std::string(hasShot ? "true" : "false")
                + "\nPos X: " + Utils::FormatFloat(transform.pos.x, 2) + " Y: " + Utils::FormatFloat(transform.pos.y, 2)
                + "\nAngle: " + Utils::FormatFloat(cannonAngle * (180.0f / 3.14159265359), 2) + " " + Utils::FormatFloat(cannonAngle, 2)
                + "\n\nSelected Projectile: " + std::to_string(selectedProjectileId) + " Mode: " + std::to_string(static_cast<int>(selectedProjectile.Mode()))
                + "\nDamage: " + std::to_string(selectedProjectile.attribute.Damage()) + " Radius: " + std::to_string(selectedProjectile.attribute.DamageRadius());
            if (!isPlayerControlled) {
                debugStr += "\nShoot Timer: " + Utils::FormatFloat(aiShootTimer, 2);
            }

            if (activeProjectiles.size() > 0) {
                Projectile& projectile = activeProjectiles.back();
                debugStr += "\n\nActive Projectile " + std::to_string(static_cast<int>(projectile.Mode())) + "\nSize: x: "
                    + Utils::FormatFloat(projectile.transform.size.x, 2) + " y: " + Utils::FormatFloat(projectile.transform.size.y, 2)
                    + "\nPos x: " + Utils::FormatFloat(projectile.transform.pos.x, 2) + " y " + Utils::FormatFloat(projectile.transform.pos.y, 2)
                    + "\nVelocity x: " + Utils::FormatFloat(activeProjectiles.back().velocity.x, 2) + " y: " + Utils::FormatFloat(projectile.velocity.y, 2)
                    + "\nGrounded: " + std::string(projectile.isGrounded ? "true" : "false")
                    + "\nTo Be Destroyed: " + std::string(projectile.toBeDestroyed ? "true" : "false")
                    + "\nTimer: " + Utils::FormatFloat(projectile.timer, 2)
                    + "\nAttack Charges: " + std::to_string(projectile.AttackCharges())
                    + "\nDamage Radius: " + std::to_string(projectile.attribute.DamageRadius());
            }

            TextObject textObj{ debugStr, Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, {0.5f, 0.99f} };
            textObj.Draw();
        }
    }

    void Cannon::Render() {
        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, cannonAngle, transform.size.x, transform.size.y);
        GraphicsManager::DrawGameObject(texture, mat, textureColor);
    }

    void Cannon::ResetTurn() {
        hasShot = false;
        SetCamera(false);
    }

    void Cannon::EnemyAI(AEVec2 const targetPos) {
        if (isActive || hasShot) return; // return if the cannon has shot something out or the projectile has already been shot out and destroyed

        if (!canAIShoot) {
            // Calculate angle and shot power first
            EnemyAICalculation(targetPos);

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
        //float dy = targetPos.y - transform.pos.y;
        //float distance = sqrt(dx * dx + dy * dy);

        // Randomise angle between the min and max radian
        float min = 30 * (PI / 180.0f);
        float max = 60 * (PI / 180.0f);
        float randomAngleBase = Utils::RandomValue(min, max);

        // Adjust for direction (Shooting left or right)
        float launchAngle = (dx > 0) ? randomAngleBase : (PI - randomAngleBase);

        // Calculate required Power (Initial Velocity)
        // Basic range formula: v = sqrt( (g * d) / sin(2 * theta) )
        // We use absolute distance for the calculation
        float neededVelocity = sqrt((Config::GRAVITY * abs(dx)) / sin(2 * randomAngleBase));

        // Randomise offset to add a variance to 
        float difficulty = 1.0f; // TODO: Adjust difficulty based on levels
        float inaccuracy = (1.0f - difficulty) + 0.05f; // add a variance value to it
        float randomOffset = Utils::RandomValue(-inaccuracy, inaccuracy);

        // Apply the variables to the cannon
        cannonAngle = launchAngle;
        shotPower = std::clamp(neededVelocity * (1.0f + randomOffset), minPower, maxPower);

        std::cout << "[CannonManager] Enemy AI Ideal: angle: " << launchAngle << " shot power: " << neededVelocity << '\n'
            << "[CannonManager] Enemy AI Actual: angle: " << cannonAngle << " shot power: " << shotPower << '\n';
    }

    void Cannon::ChargeShot() {
        if (isActive == true) return;

        if (AEInputCheckCurr(AEVK_LBUTTON) || canAIShoot == true) {
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

            // Render UI
            float x = transform.pos.x - (transform.size.x * 1.5f);
            if (!isShootingRight) x = transform.pos.x + (transform.size.x * 1.5f);
            UIManager::Bar bar { { x, transform.pos.y + (transform.size.y * 1.5f) }, 
                { 15,100 }, Color{ 0,1,0 }, UIManager::FillType::TOP_TO_BOTTOM };
            bar.Draw(chargingPower, maxPower);
        }

        // Shoot when the button is RELEASED
        if (isPlayerControlled && AEInputCheckReleased(AEVK_LBUTTON)) {
            shotPower = chargingPower;
            ShootProjectile();
            chargingPower = minPower; // Reset for next shot
        }
    }

    void Cannon::ClampAngle(float &angle) {
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

    void Cannon::SelectProjectile() {
        for (int i = 0; i < 3; ++i) {
            if (AEInputCheckTriggered(static_cast<u8>(AEVK_0 + i))) { //TEMPORARY FOR NOW USE NUMBERS INPUT
                abilityController->ChangeAbility(i);
            }
        }

        selectedProjectileId = abilityController->FindProjectileId();
    }

    void Cannon::ShootProjectile() {
        if (isActive) return;

        isActive = true;
        // Assign values to this active projectile
        Projectile& projectile = projectiles[selectedProjectileId];
        projectile.isActive = true;
        projectile.id = static_cast<int>(activeProjectiles.size() - 1);

        // Add offset to ensure that the projectile spawn at the barrel instead of the middle of the cannon
        projectile.transform.pos.x = transform.pos.x + (cos(cannonAngle) * transform.size.x);
        projectile.transform.pos.y = transform.pos.y + (sin(cannonAngle) * transform.size.y);

        // Calculate velocity here once only to ensure that gravity has an effect
        projectile.velocity.x = cos(cannonAngle) * shotPower;
        projectile.velocity.y = sin(cannonAngle) * shotPower;
        projectile.timer = Config::PROJECTILE_LIFESPAN;

        // Add to the vector list
        activeProjectiles.push_back(projectile);

        abilityController->UseAbility();

        SetCamera(true);
        std::cout << "[CannonManager] Projectile Fired!\n";
    }

    void Cannon::UpdateProjectile() {
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

                float radius = projectile.transform.size.x / 2.0f;
                float bias = 0.25f;
                int minX = gridX - 1, minY = gridY - 1, maxX = gridX + 1, maxY = gridY + 1;
                for (int y = minY; y <= maxY; ++y) {
                    for (int x = minX; x <= maxX; ++x) {
                        if (Utils::IsGridIndexOutOfBounds(x, y)) continue;

                        Tile& tile = (*MapManager::currentMap)[y][x];

                        // Checking collision of the projectile with the tile blocks
                        if (tile.IsBlocks() || tile.IsGround() || tile.IsEnemy()) {
                            if (CollisionManager::AreCircleSquareIntersecting(
                                tile.transform.pos.x, tile.transform.pos.y, tile.transform.size.x, tile.transform.size.y,
                                projectile.transform.pos.x, projectile.transform.pos.y, radius))
                            {
                                projectile.collisionTriggered = true;

                                if (projectile.isCollisionActive) {
                                    projectile.SimulatePhysics(tile);
                                }

                                if (projectile.collisionTriggered) {
                                    projectile.DamageTile(x, y);
                                    projectile.isCollisionActive = true;

                                    // If the projectile hit something, REDUCE the velocity by a percentage
                                    float percentage = 0.2f;
                                    projectile.velocity.x *= percentage;
                                    projectile.velocity.y *= percentage;
                                }
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
                            float halfW = tile.transform.size.x * 0.5f, halfH = tile.transform.size.y * 0.5f;

                            if (tile.IsBlocks() || tile.IsGround()) {
                                float tileTop = tile.transform.pos.y + halfH;
                                float tileLeft = tile.transform.pos.x - halfW;
                                float tileRight = tile.transform.pos.x + halfW;

                                // Check if projectile is resting on top of this tile by checking the projectile x-axis is within the tile x-axis
                                // and its y-axis is the same +- bias as the tile + projectile radius
                                if (projectile.transform.pos.x >= tileLeft - radius &&
                                    projectile.transform.pos.x <= tileRight + radius &&
                                    std::abs(projectile.transform.pos.y - (tileTop + radius)) < bias) {
                                    projectile.isGrounded = true;
                                    projectile.velocity.y = 0;
                                    break;
                                }
                            }
                        }
                        if (projectile.isGrounded) break;
                    }
                }
            }

            DestroyProjectile(projectile);
            projectile.Render();
        }
    }

    void Cannon::DestroyProjectile(Projectile& projectile) {
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

        // Destroy projectile and erase it from vector if the timer runs out or out of bounds
        if (projectile.timer <= 0.0f || !projectile.IsInMapBounds()) {
            projectile.Destroy();

            // Find the index containing the projectile and erase from the active ammo vector
            for (int i = 0; i < activeProjectiles.size(); i++) {
                if (activeProjectiles[i].id == projectile.id) {
                    activeProjectiles.erase(activeProjectiles.begin() + i);
                    //SetCamera(false);
                    std::cout << "[CannonManager] Remove projectile " << activeProjectiles.size() << std::endl;
                }
            }

            isActive = false; // reset cannon back to false to allow player to shoot again after projectile been destroyed
            hasShot = true;
        }
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
        int numDots = 0;
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
            float drawY = startY + (vy * t) - (0.5f * Config::GRAVITY * t * t);

            float alpha = 1.0f - static_cast<float>(i / numDots);
            GraphicsManager::DrawGameObject(AssetsManager::circleTexture, GraphicsManager::CreateTransformMatrix(drawX, drawY, 0, 10, 10), { 1,1,1, alpha });
        }
    }
}