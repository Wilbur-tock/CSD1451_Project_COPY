#include "CannonManager.hpp"

namespace CannonManager {
    Projectile::Projectile() : timer{ 0.0f }, velocity{ 0.0f,0.0f } {}
    Projectile::Projectile(AEVec2 _pos, AEGfxTexture* _texture, AEVec2 _velocity) 
        : GameObject(_pos, defaultSize, _texture), timer{ 0.0f }, velocity(_velocity) {}

    void Projectile::Render() {
        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, 0, size.x, size.y);
        GraphicsManager::DrawGameObject(AssetsManager::circleTexture, mat, Color{ 0.5f, 0.5f, 0, 1 });
    }

    void Projectile::MoveProjectile(float dt) {
        if (isGrounded) {
            velocity.x *= pow(Config::FRICTION, dt);
            velocity.y = 0; // ensure that the projectile stay on the ground
            if (std::abs(velocity.x) < 1.0f) velocity.x = 0;
        }
        else {
            velocity.y -= Config::GRAVITY * dt;
        }

        pos.x += velocity.x * dt;
        pos.y += velocity.y * dt;
    }

    void Projectile::ConsumeAttackCharge() {

    }

    void Projectile::Destroy() {
        isActive = collisionDetected = toBeDestroyed = false;
        timer = 0.0f;
    }

    Cannon::Cannon() : ammoLimit {10} { Reset(); }
    Cannon::Cannon(AEVec2 _pos, AEGfxTexture* _texture, int _ammoLimit, AEVec2 _size)
        : GameObject(_pos, _size, _texture, Color{ 1,1,1,1 }),
        ammoLimit{ _ammoLimit }, cannonAngle{ 0.0f }, shotPower{ power } {
        Reset();
    }

    float Cannon::GetDamage() const { return damage; }
    void Cannon::SetDamage(float _damage) { 
        damage = _damage;
        Utils::Limit(damage, minDamage, maxDamage); 
    }

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
        Utils::Limit(cannonAngle, -1.5f, 1.5f);

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
            
            TextObject txtObj{ "", Utils::debuggerInfo.first, Utils::debuggerInfo.second, {0.5f, 0.99f} };
            txtObj.textContent = "Cannon\nPos X: " + Utils::FormatFloat(pos.x, 2) + " Y: " + Utils::FormatFloat(pos.y, 2)
                + "\nAngle: " + Utils::FormatFloat(cannonAngle * (180.0f / 3.14159265359), 2);
            if (activeAmmo.size() > 0) {
                Projectile& projectile = activeAmmo.back();
                txtObj.textContent += "\nActive Projectile:\nSize: x: "
                    + Utils::FormatFloat(projectile.size.x, 2) + " y: " + Utils::FormatFloat(projectile.size.y, 2)
                    +"\nVelocity x : " + Utils::FormatFloat(activeAmmo.back().velocity.x, 2)
                    + " y: " + Utils::FormatFloat(projectile.velocity.y, 2)
                    + "\nGrounded: " + std::string(projectile.isGrounded ? "true" : "false")
                    + "\nTo Be Destroyed: " + std::string(projectile.toBeDestroyed ? "true" : "false")
                    + "\nTimer: " + Utils::FormatFloat(projectile.timer,2);
            }
            
            GraphicsManager::DrawTextUI(txtObj);
        }
        Render();
    }

    void Cannon::ChargeShot() {
        if (AEInputCheckCurr(VK_LBUTTON)) {
            float chargeRateWithDT = chargeRate * GameStateManager::dt;
            power += isPowerIncrease ? chargeRateWithDT : -chargeRateWithDT;

            if (power > maxPower) {
                power = maxPower;
                isPowerIncrease = false;
            }
            if (power < minPower) {
                power = minPower;
                isPowerIncrease = true;
            }

            UIChargeShotBar();
        }

        // Shoot when the button is RELEASED
        if (AEInputCheckReleased(VK_LBUTTON)) {
            shotPower = power;
            ShootProjectile(); // Pass the built-up power
            power = minPower; // Reset for next shot
        }
    }

    void Cannon::Render() {
        // size.x/size.y are your "half-size" scales (same convention as your tiles use tileSize/2)
        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, cannonAngle, size.x, size.y);
        GraphicsManager::DrawGameObject(texture, mat, textureColor);
    }

    void Cannon::ShootProjectile() {
        // Assign values to an projectile
        ammo[ammoCount].isActive = true;
        ammo[ammoCount].id = ammoCount;
        ammo[ammoCount].mode = ATTACK;

        // Add offset to ensure that the projectile spawn at the barrel instead of the middle of the cannon
        ammo[ammoCount].pos.x = pos.x + cos(cannonAngle) * size.x;
        ammo[ammoCount].pos.y = pos.y + sin(cannonAngle) * size.x;

        // Calculate velocity here once only to ensure that gravity has an effect
        ammo[ammoCount].velocity.x = cos(cannonAngle) * shotPower;
        ammo[ammoCount].velocity.y = sin(cannonAngle) * shotPower;
        ammo[ammoCount].timer = projectileLifespan;

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

                bool wasGrounded = projectile.isGrounded;
                projectile.isGrounded = false;

                // Check for collision in a 3x3 area with the projectile
                int gridX = static_cast<int>((projectile.pos.x + Config::WORLD_MAX_X) / Config::TILE_SIZE);
                int gridY = static_cast<int>((Config::WORLD_MAX_Y - projectile.pos.y) / Config::TILE_SIZE);

                int maxY = gridY + 1, maxX = gridX + 1;
                for (int y = gridY - 1; y <= maxY; ++y) {
                    for (int x = gridX - 1; x <= maxX; ++x) {
                        if (x >= 0 && x < Config::GRID_WIDTH && y >= 0 && y < Config::GRID_HEIGHT) {
                            MapManager::Tile& tile = (*MapManager::currentMap)[y][x];

                            if (tile.IsBlocks() || tile.IsGround()) {
                                float radius = projectile.size.x / 2.0f;

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

                                    float bias = 0.1f;
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

                                        // Ensure that projectile stays grounded when it hit the side of the wall 
                                        // if it is already grounded
                                        if (wasGrounded) {
                                            projectile.isGrounded = true;
                                            projectile.velocity.y = 0;
                                        }
                                    }
                                    else {
                                        // Resolve Y (Floor/Ceiling) - Hit the top or bottom of the block
                                        if (projectile.pos.y > tile.pos.y) { // Ensure the projectile is flush to the tile
                                            projectile.pos.y = tileTop + radius;
                                            projectile.isGrounded = true;
                                            projectile.velocity.y *= -0.2f;
                                            if (std::abs(projectile.velocity.y) < 0.5f) projectile.velocity.y = 0;
                                        }
                                        else { // If it hits the celling fall down
                                            projectile.pos.y = tileBottom - radius;
                                            projectile.velocity.y *= -0.5f;
                                        }
                                    }
                                    projectile.collisionDetected = true;

                                    // Only deal damage once
                                    if (tile.IsBlocks() && projectile.mode == ATTACK) {
                                        tile.Damage(damage);
                                        projectile.mode = IDLE;
                                    }
                                }
                            }
                        }
                    }
                }

                // SECONDARY CHECK IF THE PROJECTILE IS GROUNDED OR NOT
                if (!projectile.isGrounded) {
                    float radius = projectile.size.x / 2.0f;

                    for (int y = gridY - 1; y <= maxY; ++y) {
                        for (int x = gridX - 1; x <= maxX; ++x) {
                            if (x >= 0 && x < Config::GRID_WIDTH && y >= 0 && y < Config::GRID_HEIGHT) {
                                MapManager::Tile& tile = (*MapManager::currentMap)[y][x];

                                if (tile.IsBlocks() || tile.IsGround()) {
                                    float tileTop = tile.pos.y + (tile.size.y / 2.0f);
                                    float tileLeft = tile.pos.x - (tile.size.x / 2.0f);
                                    float tileRight = tile.pos.x + (tile.size.x / 2.0f);

                                    // Check if projectile is resting on top of this tile
                                    if (projectile.pos.x >= tileLeft - radius &&
                                        projectile.pos.x <= tileRight + radius &&
                                        std::abs(projectile.pos.y - (tileTop + radius)) < 0.3f) {
                                        projectile.isGrounded = true;
                                        projectile.velocity.y = 0;
                                        break;
                                    }
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
        if (projectile.collisionDetected && projectile.toBeDestroyed == false && speedSq <= 0.1f) {
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

        float percentage = (power / maxPower);
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
}