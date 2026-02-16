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

namespace CannonManager {
    Projectile::Projectile() : timer{ 0.0f }, velocity{ 0.0f,0.0f } {}
    Projectile::Projectile(AEVec2 _pos, AEGfxTexture* _texture, AEVec2 _velocity) 
        : GameObject(_pos, defaultSize, _texture), timer{ 0.0f }, velocity(_velocity) {}

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

    //======================================= Cannon ====================================

    Cannon::Cannon() : ammoLimit {10} { Reset(); }
    Cannon::Cannon(AEVec2 _pos, AEGfxTexture* _texture, int _ammoLimit, bool _isShootingRight, AEVec2 _size)
        : GameObject(_pos, _size, _texture, Color{ 1,1,1,1 }),
        ammoLimit{ _ammoLimit }, isShootingRight{_isShootingRight}, cannonAngle{0.0f}, ammoCount{ 0 } {
        Reset();
    }

    int Cannon::Damage() const { return damage; }
    void Cannon::Damage(int _damage) { damage = std::clamp(_damage, minDamage, maxDamage); }

    void Cannon::Reset() {
        // Reset to default
        ammo.clear();
        ammo.resize(ammoLimit);
        // Initalize all the projectile variables
        for (Projectile &projectile : ammo) {
            projectile = { {0,0}, AssetsManager::circleTexture, {0,0} };
            projectile.size = { 10,10 };
        }
        activeAmmo.clear(); // ensure that there should not be any active ammo when cannon is reset
        ammoCount = 0;
    }

    void Cannon::Update() {
        float worldMouseX = 0, worldMouseY = 0;
        Utils::GetWorldMousePos(worldMouseX, worldMouseY);
        
        // Calculate angle toward mouse
        cannonAngle = atan2(worldMouseY - pos.y, worldMouseX - pos.x);

        // Add limits so it can only turn facing upwards or downwards limits
        if (isShootingRight) {
            cannonAngle = std::clamp(cannonAngle, -1.5f, 1.5f);
        }
        else {
            // Clamp to left hemisphere
            if (cannonAngle > 0) {
                cannonAngle = std::clamp(cannonAngle, 1.5f, 3.5f);   // Upper left: 1.5 to 3.5
            }
            else {
                cannonAngle = std::clamp(cannonAngle, -3.5f, -1.5f); // Lower left: -3.5 to -1.5
            }
        }

        DrawTrajectory();
        ChargeShot();
        UpdateProjectile();

        // Debugger
        if (Utils::isDebuggerOn) {
            float mX = 0, mY = 0;
            Utils::GetWorldMousePos(mX, mY);
            float cX = pos.x, cY = pos.y;
            Utils::GetWorldPos(cX, cY);
            s32 mouseX, mouseY;
            AEInputGetCursorPosition(&mouseX, &mouseY);
            
            TextObject txtObj{ "", Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, {0.5f, 0.99f} };
            txtObj.textContent = "Cannon\nPos X: " + Utils::FormatFloat(pos.x, 2) + " Y: " + Utils::FormatFloat(pos.y, 2)
                + "\nAngle: " + Utils::FormatFloat(cannonAngle * (180.0f / 3.14159265359), 2) + " " + Utils::FormatFloat(cannonAngle,2)
                + "\nDamage: " + Utils::FormatFloat(damage,2);
            if (activeAmmo.size() > 0) {
                Projectile& projectile = activeAmmo.back();
                txtObj.textContent += "\nActive Projectile:\nSize: x: "
                    + Utils::FormatFloat(projectile.size.x, 2) + " y: " + Utils::FormatFloat(projectile.size.y, 2)
                    + "\nPos x: " + Utils::FormatFloat(projectile.pos.x, 2) + " y " + Utils::FormatFloat(projectile.pos.y, 2)
                    + "\nVelocity x: " + Utils::FormatFloat(activeAmmo.back().velocity.x, 2) + " y: " + Utils::FormatFloat(projectile.velocity.y, 2)
                    + "\nGrounded: " + std::string(projectile.isGrounded ? "true" : "false")
                    + "\nTo Be Destroyed: " + std::string(projectile.toBeDestroyed ? "true" : "false")
                    + "\nTimer: " + Utils::FormatFloat(projectile.timer,2)
                    + "\nAttack Charges: " + std::to_string(projectile.AttackCharges());
            }
            
            GraphicsManager::DrawTextUI(txtObj);
        }
        Render();
    }

    void Cannon::ChargeShot() {
        if (isActive == true) return;

        if (AEInputCheckCurr(VK_LBUTTON)) {
            float chargeRateWithDT = chargeRate * GameStateManager::dt;
            chargingPower += isPowerIncrease ? chargeRateWithDT : -chargeRateWithDT;

            if (chargingPower > maxPower) {
                chargingPower = maxPower;
                isPowerIncrease = false;
            }
            if (chargingPower < minPower) {
                chargingPower = minPower;
                isPowerIncrease = true;
            }

            // When player press left mouse click, set camera to position the cannon and unable to move the camera
            CameraManager::camera.SetPosition(pos);
            CameraManager::camera.SetState(CameraManager::CameraController::CameraState::STATIC);
            UIChargeShotBar();
        }

        // Shoot when the button is RELEASED
        if (AEInputCheckReleased(VK_LBUTTON)) {
            isActive = true;
            shotPower = chargingPower;
            ShootProjectile();
            chargingPower = minPower; // Reset for next shot
        }
    }

    void Projectile::DamageBlocks(int x, int y, int damage, int damageRadiusMultipler) {
        if (Mode() == IDLE) return;

        int minDX = x, minDY = y, maxDX = x, maxDY = y;
        // Increased damage radius
        if (damageRadiusMultipler > 1) {
            int addRadius = damageRadiusMultipler - 1;
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
                    << " " << tileToDamage.attributes.GetHealth() << std::endl;

                tileToDamage.Damage(damage);
                ConsumeAttackCharge();
            }
        }
    }

    void Cannon::Render() {
        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, cannonAngle, size.x, size.y);
        GraphicsManager::DrawGameObject(texture, mat, textureColor);
    }

    void Cannon::ShootProjectile() {
        // Assign values to this active projectile
        Projectile& projectile = ammo[ammoCount];
        projectile.isActive = true;
        projectile.id = ammoCount;
        projectile.Mode(ATTACK);

        // Add offset to ensure that the projectile spawn at the barrel instead of the middle of the cannon
        projectile.pos.x = pos.x + (cos(cannonAngle) * size.x);
        projectile.pos.y = pos.y + (sin(cannonAngle) * size.y);

        // Calculate velocity here once only to ensure that gravity has an effect
        projectile.velocity.x = cos(cannonAngle) * shotPower;
        projectile.velocity.y = sin(cannonAngle) * shotPower;
        projectile.timer = projectileLifespan;

        // Add to the vector list
        activeAmmo.push_back(ammo[ammoCount]);

        // Once it reach the ammo limit reset the ammo count
        if (++ammoCount >= ammoLimit) {
            ammoCount = 0;
        }

        SetCamera(true);
        std::cout << "Projectile Fired!" << " " << ammoCount << std::endl;
    }

    void Cannon::UpdateProjectile() {
        for (Projectile& projectile : activeAmmo) {
            if (!projectile.isActive) continue;

            float dt = GameStateManager::dt;
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
                Utils::GetGridIndex(gridX, gridY, projectile.pos);

                float radius = projectile.size.x / 2.0f;
                float bias = 0.25f;
                int minX = gridX - 1, minY = gridY - 1, maxX = gridX + 1, maxY = gridY + 1;
                for (int y = minY; y <= maxY; ++y) {
                    for (int x = minX; x <= maxX; ++x) {
                        if (Utils::IsGridIndexOutOfBounds(x, y)) continue;

                        Tile& tile = (*MapManager::currentMap)[y][x];

                        if (tile.IsBlocks() || tile.IsGround()) {
                            if (CollisionManager::AreCircleSquareIntersecting(
                                tile.pos.x, tile.pos.y, tile.size.x, tile.size.y,
                                projectile.pos.x, projectile.pos.y, radius))
                            {
                                // Bounds
                                float tileTop = tile.pos.y + (tile.size.y / 2.0f);
                                float tileBottom = tile.pos.y - (tile.size.y / 2.0f);
                                float tileLeft = tile.pos.x - (tile.size.x / 2.0f);
                                float tileRight = tile.pos.x + (tile.size.x / 2.0f);

                                float overlapX = (radius + tile.size.x / 2.0f) - std::abs(projectile.pos.x - tile.pos.x);
                                float overlapY = (radius + tile.size.y / 2.0f) - std::abs(projectile.pos.y - tile.pos.y);

                                if (overlapX < (overlapY + bias)) {
                                    float bounce = -0.3f;
                                    // Resolve X (Wall) - Hit a side wall
                                    if (projectile.pos.x < tile.pos.x) {
                                        projectile.pos.x = tileLeft - radius - 0.1f; // Nudge out
                                        if (projectile.velocity.x > 0) projectile.velocity.x *= bounce;
                                    }
                                    else {
                                        projectile.pos.x = tileRight + radius + 0.1f; // Nudge out
                                        if (projectile.velocity.x < 0) projectile.velocity.x *= bounce;
                                    }
                                }
                                else {
                                    // Resolve Y (Floor/Ceiling) - Hit the top or bottom of the block
                                    if (projectile.pos.y > tile.pos.y) {
                                        projectile.pos.y = tileTop + radius; // Flush to the tile
                                        projectile.isGrounded = true;
                                        projectile.velocity.y *= -0.2f; // Lose 80% of its velocity
                                        if (std::abs(projectile.velocity.y) < Config::MINIMUM_VELOCITY) projectile.velocity.y = 0; // prevent tiny velocity
                                    }
                                    else { // If it hits the celling fall down
                                        projectile.pos.y = tileBottom - radius;
                                        projectile.velocity.y *= -0.5f; // Lost 50% of its velocity
                                    }
                                }
                                projectile.isCollisionDetected = true;

                                if (projectile.isCollisionDetected) {
                                    projectile.DamageBlocks(x, y, damage, damageTileRadiusMultipler);
                                }

                                // Damaged tiles once a collision with a block/ground is detected
                                //if (projectile.isCollisionDetected && projectile.Mode() == ATTACK) {
                                //    int minDX = x, minDY = y, maxDX = x, maxDY = y;
                                //    // Increased damage radius
                                //    if (damageTileRadiusMultipler > 1) {
                                //        int addRadius = damageTileRadiusMultipler - 1;
                                //        minDX -= addRadius;
                                //        minDY -= addRadius;
                                //        maxDX += addRadius;
                                //        maxDY += addRadius;
                                //    }
                                //
                                //    for (int dY = minDY; dY <= maxDY; ++dY) {
                                //        for (int dX = minDX; dX <= maxDX; ++dX) {
                                //            if (Utils::IsGridIndexOutOfBounds(dX, dY)) continue;
                                //
                                //            MapManager::Tile& tileToDamage = (*MapManager::currentMap)[dY][dX];
                                //            // Only damage blocks type
                                //            if (!tileToDamage.IsBlocks()) continue;
                                //
                                //            tileToDamage.Damage(damage);
                                //            projectile.ConsumeAttackCharge();
                                //
                                //            std::cout << "Damage Tile: " << MapManager::TileTypeToString(tile.type)
                                //                << " x: " << tile.gridId.x << " y: " << tile.gridId.y
                                //                << " " << tile.attributes.GetHealth() << std::endl;
                                //        }
                                //    }
                                //}
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

                            if (tile.IsBlocks() || tile.IsGround()) {
                                float tileTop = tile.pos.y + (tile.size.y / 2.0f);
                                float tileLeft = tile.pos.x - (tile.size.x / 2.0f);
                                float tileRight = tile.pos.x + (tile.size.x / 2.0f);

                                // Check if projectile is resting on top of this tile by checking the projectile x-axis is within the tile x-axis
                                // and its y-axis is the same +- bias as the tile + projectile radius
                                if (projectile.pos.x >= tileLeft - radius &&
                                    projectile.pos.x <= tileRight + radius &&
                                    std::abs(projectile.pos.y - (tileTop + radius)) < bias) {
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
        if (projectile.isCollisionDetected && projectile.toBeDestroyed == false && speedSq <= 0.1f) {
            projectile.timer = projectileLifespan;
            projectile.toBeDestroyed = true;
        }

        if (projectile.toBeDestroyed) {
            projectile.timer -= 1.0f * GameStateManager::dt;
        }

        // Destroy projectile and erase it from vector if the timer runs out or out of bounds
        if (projectile.timer <= 0.0f || !projectile.IsInMapBounds()) {
            projectile.Destroy();

            // Find the index containing the projectile and erase from the active ammo vector
            for (int i = 0; i < activeAmmo.size(); i++) {
                if (activeAmmo[i].id == projectile.id) {
                    activeAmmo.erase(activeAmmo.begin() + i);
                    SetCamera(false);
                    std::cout << "Removing " << i << " " << "Count " << activeAmmo.size() << std::endl;
                }
            }

            isActive = false; // reset cannon back to false to allow player to shoot again after projectile been destroyed
        }
    }

    void Cannon::UIChargeShotBar() {
        AEGfxTexture* tex = AssetsManager::squareTexture;

        float x = pos.x - (defaultSize.x * 1.75f), y = pos.y;
        float w = 20, h = 75, gap = 5;

        // Background Border Bar
        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(x, y, 0, w, -h);
        GraphicsManager::DrawUI(tex, mat, Color{ 0, 0, 0, 1 });

        w = w - (gap * 2), h = h - (gap * 2);

        x += gap, y += gap;

        // Lighter background bar
        AEMtx33 mat2 = GraphicsManager::CreateTransformMatrix(x, y, 0, w, -h);
        GraphicsManager::DrawUI(tex, mat2, Color{ 0, 0.2f, 0, 1 });

        float percentage = (chargingPower / maxPower);
        float newH = h * percentage;

        // Bar
        AEMtx33 mat3 = GraphicsManager::CreateTransformMatrix(x, y, 0, w, -newH);
        GraphicsManager::DrawUI(tex, mat3, Color{ 0, 0.8f, 0, 1 });
    }

    void Cannon::SetCamera(bool isFollowingProjectile) {
        if (isFollowingProjectile) {
            // Set camera to follow the newly fired projectile
            CameraManager::camera.SetFollowTarget(&activeAmmo.back().pos);
            CameraManager::camera.SetState(CameraManager::CameraController::CameraState::FOLLOW);
        }
        else {
            // Return camera to input control
            CameraManager::camera.SetFollowTarget(nullptr);
            CameraManager::camera.SetState(CameraManager::CameraController::CameraState::INPUT);
            CameraManager::camera.SetPosition(pos);
        }
    }

    void Cannon::DrawTrajectory() {
        int numDots = 0;
        // Starting position (at the barrel tip, matching ShootProjectile)
        float startX = pos.x + cos(cannonAngle) * size.x;
        float startY = pos.y + sin(cannonAngle) * size.x;

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