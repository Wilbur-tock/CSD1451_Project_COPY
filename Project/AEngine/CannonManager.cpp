#include "CannonManager.hpp"

namespace CannonManager {
    Projectile::Projectile() : timer{ 0.0f }, velocity{ 0.0f,0.0f } {}
    Projectile::Projectile(AEVec2 _pos, AEGfxTexture* _texture, AEVec2 _velocity) : GameObject(_pos, defaultSize, _texture), timer{ 0.0f }, velocity(_velocity) {}

    TextObject debugText{ "",Utils::debuggerInfo.second, {-1, 0.99f}, Utils::debuggerInfo.first };

    void Projectile::Render() {
        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, 0, size.x, size.y);
        GraphicsManager::DrawGameObject(AssetsManager::circleTexture, mat, Color{ 0.5f, 0.5f, 0, 1 });
    }

    void Projectile::MoveProjectile() {
        float dt = GameStateManager::dt;
        AEVec2 newPos = { velocity.x * dt, velocity.y * dt };
        pos.x += newPos.x;
        pos.y += newPos.y;
    }

    void Projectile::Destroy(int &projectileId) {
        isActive = collisionDetected = toBeDestroyed = false;
        timer = 0.0f;
        projectileId = -1;
    }

    Cannon::Cannon() : cannonAngle{ 0.0f }, ammo{ NULL }, ammoLimit{ 0 }, shotPower{ power } {}
    Cannon::Cannon(AEVec2 _pos, AEGfxTexture* _texture, int _projectileAmount)
        : GameObject(_pos, defaultSize, _texture, Color{ 1,1,1,1 }),
        ammoLimit{ _projectileAmount }, cannonAngle{ 0.0f }, shotPower{ power } {
        Reset();
    }

    Cannon::Cannon(AEVec2 _pos, AEGfxTexture* _texture, int _projectileAmount, AEVec2 _size)
        : GameObject(_pos, _size, _texture, Color{ 1,1,1,1 }),
        ammoLimit{ _projectileAmount }, cannonAngle{ 0.0f }, shotPower{ power } {
        Reset();
    }

    void Cannon::Reset() {
        ammo.clear();
        ammo.resize(ammoLimit);
        // Initalize all the projectile variables
        for (Projectile &projectile : ammo) {
            projectile = { {0,0}, AssetsManager::circleTexture, {0,0} };
            projectile.size = { 10,10 };
        }
        activeAmmo.clear();
        ammoCount = 0;
    }

    void Cannon::Update() {
        float worldMouseX = 0, worldMouseY = 0;
        Utils::GetWorldMousePos(worldMouseX, worldMouseY);

        // Calculate angle toward mouse
        cannonAngle = atan2(worldMouseY - pos.y, worldMouseX - pos.x);

        // Add limits so it can only turn facing upwards or downwards limits
        Utils::Limit(cannonAngle, -1.5f, 1.5f);

        // Debugger
        if (Utils::isDebuggerOn) {
            float mX = 0, mY = 0;
            Utils::GetWorldMousePos(mX, mY);
            float cX = pos.x, cY = pos.y;
            Utils::GetWorldPos(cX, cY);
            s32 mouseX, mouseY;
            AEInputGetCursorPosition(&mouseX, &mouseY);

            debugText.textContent = "Cannon\nPos X: " + Utils::FormatFloat(pos.x, 2) + " Y: " + Utils::FormatFloat(pos.y, 2)
                + "\nWorld Pos X: " + Utils::FormatFloat(cX, 2) + " Y: " + Utils::FormatFloat(cY, 2)
                + "\nAngle: " + Utils::FormatFloat(cannonAngle * (180.0f / 3.14159265359), 2)
                + "\nMouse\nScreen Pos X: " + Utils::FormatFloat(mouseX, 2) + " Y: " + Utils::FormatFloat(mouseY, 2)
                + "\nWorld\nPos X: " + Utils::FormatFloat(worldMouseX, 2) + " Y : " + Utils::FormatFloat(worldMouseY, 2);

            GraphicsManager::DrawTextUI(debugText);
        }

        Render();
        ChargeShot();
        UpdateProjectile();
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

        projectileToFollowId = ammoCount;

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
            if (!projectile.isActive == true) continue;

            float dt = GameStateManager::dt;
            // Increased the frequency of the collision check by using loops
            // ensuring faster moving projectiles are check for collision multiple times
            // to ensure that the GHOSTING of block bug does not happen
            // Using sub_steps to ensure that it still moving at a per frame rate time (dt or 1/10 dt but 10 loops) even though it is looping
            const int SUB_STEPS = 10;
            float subDt = dt / SUB_STEPS;

            bool hasDealDamage = false;

            for (int step = 0; step < SUB_STEPS; ++step) {
                projectile.isGrounded = false;

                //projectile.MoveProjectile();
                projectile.pos.x += projectile.velocity.x * subDt;
                projectile.pos.y += projectile.velocity.y * subDt;

                // Check for collision in a 3x3 area with the projectile
                int gridX = static_cast<int>((projectile.pos.x + Config::WORLD_MAX_X) / Config::TILE_SIZE);
                int gridY = static_cast<int>((Config::WORLD_MAX_Y - projectile.pos.y) / Config::TILE_SIZE);

                bool collisionFoundInStep = false; // to prevent GHOSTING of blocks bug

                int maxY = gridY + 1, maxX = gridX + 1;
                for (int y = gridY - 1; y <= maxY && !collisionFoundInStep; ++y) {
                    for (int x = gridX - 1; x <= maxX && !collisionFoundInStep; ++x) {
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

                                    if (overlapX < overlapY) {
                                        // Resolve X (Wall)
                                        if (projectile.pos.x < tile.pos.x) {
                                            projectile.pos.x = tileLeft - radius - 0.1f; // Nudge out
                                            if (projectile.velocity.x > 0) projectile.velocity.x *= -0.4f;
                                        }
                                        else {
                                            projectile.pos.x = tileRight + radius + 0.1f; // Nudge out
                                            if (projectile.velocity.x < 0) projectile.velocity.x *= -0.4f;
                                        }
                                    }
                                    else {
                                        // Resolve Y (Floor/Ceiling)
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
                                    collisionFoundInStep = true;

                                    // Only deal damage once
                                    if (tile.IsBlocks() && !hasDealDamage && projectile.mode == ATTACK) {
                                        tile.Damage(damage);
                                        hasDealDamage = true;
                                        projectile.mode = IDLE;
                                    }
                                }
                            }
                        }
                    }
                }

                if (projectile.isGrounded) {
                    projectile.velocity.x *= pow(Config::FRICTION, subDt);
                    if (std::abs(projectile.velocity.x) < 0.1f) projectile.velocity.x = 0;
                    projectile.velocity.y = 0;
                }
                else {
                    projectile.velocity.y -= Config::GRAVITY * subDt;
                }
            }

            DestroyProjectile(projectile);
            projectile.Render();
        }
    }

    void Cannon::DestroyProjectile(Projectile& projectile) {
        // Start timer if it going to be destroy
        if (projectile.collisionDetected && projectile.toBeDestroyed == false) {
            projectile.timer = 5.0f;
            projectile.toBeDestroyed = true;
        }

        if (projectile.toBeDestroyed) {
            projectile.timer -= 1.0f *GameStateManager::dt;
        }

        // Destroy projectile and erase it from vector if the timer runs out or out of bounds
        if (projectile.timer <= 0.0f || !projectile.IsInMapBounds()) {
            projectile.Destroy(projectileToFollowId);

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