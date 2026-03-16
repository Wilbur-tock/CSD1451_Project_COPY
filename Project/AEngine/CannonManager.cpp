#include "CannonManager.hpp"

#include "Utils.hpp"
#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "CameraManager.hpp"
#include "GameStateManager.hpp"
#include "CollisionManager.hpp"
#include "Tile.hpp"
#include "AbilitiesManager.hpp"
#include "UIManager.hpp"
#include "LevelManager.hpp"
#include "PhysicsManager.hpp"
#include "AudioManager.hpp"
#include "GameTrackingManager.hpp"
#include "EffectsManager.hpp"

namespace CannonManager {
    Cannon::Cannon(AEVec2 _pos, AEGfxTexture* _texture, TileIndex _tileId, AEVec2 _size, bool _isShootingRight, bool _isPlayerControlled)
        : GameObject(_pos, _size, _texture, _tileId, Color{ 1,1,1,1 }), isShootingRight{ _isShootingRight }, isPlayerControlled{ _isPlayerControlled } {
    }

    TextObject debugText;

    void Cannon::Initialize(AbilitiesManager::AbilityController* controller) {
        if (!controller) {
            std::cerr << "[CannonManager] abilityController is nullptr!" << '\n';
            return;
        }

        abilityController = controller;
        controller->ChangeAbility(0); // Set default projectile as the default ability
        ProjectileSettings();

        //Initialize UI Objects
        debugText = { "", Utils::debuggerInfo.first * 0.8f, Utils::debuggerInfo.second, { 0.5f, 0.99f } };

        float x = transform.pos.x - (transform.size.x * 1.5f);
        if (!isShootingRight) x = transform.pos.x + (transform.size.x * 1.5f);

        worldSpaceShotBar = UIManager::Bar{ { x, transform.pos.y + (transform.size.y * 1.5f) },
            { 15,100 }, Color{ 0,1,0 }, UIManager::FillType::TOP_TO_BOTTOM };

        Transform trans{ { 0,0 }, {600,15} };
        Utils::SetAnchoredPosition(trans, Utils::AnchorMode::BOTTOM_CENTER);
        trans.pos.y += 100 + 15;
        screenOverlayShotBar = UIManager::Bar{ trans.pos, trans.size, Color{ 0,1,0 }, UIManager::FillType::LEFT_TO_RIGHT };
    }
    
    void Cannon::ProjectileSettings() {
        std::vector<Ability> const& allProjectileAbilities = abilityController->GetProjectilesAbilties();
        projectileAbilitiesCount = static_cast<int>(allProjectileAbilities.size());
        // Reset to default
        activeProjectiles.clear(); // ensure that there should not be any active ammo when cannon is reset
        activeProjectiles.reserve(projectileAbilitiesCount);
        projectiles.clear();
        projectiles.resize(projectileAbilitiesCount);

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
            projectile = Projectile{ AssetsManager::circleTexture };
            projectile.SetProjectileStats(ability.GetName(), ability.GetProj());

            //std::cout << "[CannonManager] AbilityId: " << ability.Id() << " Projectile Name: " << projectile.ProjectileName() << " Projectile Mode: " << projectile.ModeName() << " "
            // << projectile.ProjectileId() << " into the cannon projectile vector" << '\n';
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

        SelectProjectileType();
        ChargeShot();
        UpdateProjectile();
        DebugCommands();
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
            // Drawing power bar beside the cannon in WORLD SPACE
            worldSpaceShotBar.Draw(chargingPower, Config::CANNON_MAX_POWER, false);

            if (!isPlayerControlled) return;
             
            // Only draw power bar in the SCREEN SPACE for player cannon
            screenOverlayShotBar.Draw(chargingPower, Config::CANNON_MAX_POWER);
        }
    }

    void Cannon::ResetForNewTurn() {
        allProjectileDestroyed = false;
        hasCannonShot = false;
        SetCamera(false);

        // ALWAYS Reset cannon ability back to default after at the start off a a new turn
        abilityController->ResetForNewTurn();

        // Reselect AI ability
        if (!isPlayerControlled && abilityController) {
            abilityController->ResetAIAbilitySelection();
        }
    }

    bool Cannon::IsTargetAlive() {
        if (!target) return false;

        // Remove target ptr if current target has already been destroyed
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
            aiDifficulty = 0.95f;
            break;
        case 1: // Medium
            aiDifficulty = 0.975f;
            break;
        case 2: // Hard
            aiDifficulty = 1.0f;
            break;
        }
    }

    void Cannon::EnemyAI() {
        if (hasCannonShot || allProjectileDestroyed) return;
        if (!target) {
            std::cerr << "[CannonManager] Can't find any player character targets for the enemy to shoot!" << '\n';
            return;
        }

        if (!canAIShoot) {
            abilityController->AIAbilitySelection();
            EnemyAICalculation();

            // Set a random shoot timer to add a delay between the start of the enemy turn and enemy shooting
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

    void Cannon::EnemyAICalculation() {
        float perfectAngle = 0, perfectPower = 0;
        PhysicsManager::PerfectShotCalculation(transform, target->transform, perfectAngle, perfectPower);

        // Randomise offset to add a variance to 
        float inaccuracy = (1.0f - aiDifficulty);
        float randomOffset = Utils::RandomValue(-inaccuracy, inaccuracy);

        // Apply the variables to the cannon
        cannonAngle = perfectAngle;
        shotPower = std::clamp(perfectPower * (1.0f + randomOffset), Config::CANNON_MIN_POWER, Config::CANNON_MAX_POWER);

        std::cout << "======================= [CannonManager] Enemy AI Shot ========================\n"
            << "[CannonManager] Enemy AI Target pos x: " << target->transform.pos.x << " y: " << target->transform.pos.y << '\n'
            << "[CannonManager] Enemy AI Ideal: angle: " << perfectAngle << " shot power: " << perfectPower << '\n'
            << "[CannonManager] Enemy AI Actual: angle: " << cannonAngle << " shot power: " << shotPower << '\n';
    }

    void Cannon::ChargeShot() {
        if (!isPlayerControlled && hasCannonShot == true) return; // return if it not player controlled

        // Shoot when the button is RELEASED
        if (isPlayerControlled && AEInputCheckReleased(AEVK_LBUTTON)) {
            if (isHoldingLeftClick) {
                shotPower = chargingPower;
                ShootProjectile();
            }

            // Reset for next shot
            chargingPower = Config::CANNON_MIN_POWER;
            canPlayerShoot = true;
        }

        // Cancel player shooting if right mouse click is input
        if (isHoldingLeftClick && AEInputCheckTriggered(AEVK_RBUTTON)) {
            canPlayerShoot = isHoldingLeftClick = false;
        }

        if (!canPlayerShoot) return; // return if is player controlled and they can't shoot

        if (AEInputCheckCurr(AEVK_LBUTTON) || canAIShoot == true) {
            isHoldingLeftClick = canPlayerShoot = true;

            float chargeRateWithDT = chargingRate * PhysicsManager::g_dt;
            chargingPower += isPowerIncreasing ? chargeRateWithDT : -chargeRateWithDT;

            if (chargingPower > Config::CANNON_MAX_POWER) {
                chargingPower = Config::CANNON_MAX_POWER;
                isPowerIncreasing = false;
            }
            if (chargingPower < Config::CANNON_MIN_POWER) {
                chargingPower = Config::CANNON_MIN_POWER;
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

    void Cannon::SelectProjectileType() {
        if (!abilityController) {
            std::cerr << "[CannonManager] Can't find any abilityController" << '\n';
            return;
        }

        selectedProjectileId = abilityController->GetProjectileId();
    }

    void Cannon::ShootProjectile() {
        if (hasCannonShot) return;

        // IMPORTANT: use a COPY here so we don't permanently modify the template projectile inside projectiles[].
        Projectile projectile = projectiles[selectedProjectileId];

        AbilityActivation(projectile);

        projectile.Initialize(transform, static_cast<int>(activeProjectiles.size()), cannonAngle, shotPower);

        // Add the these projectile to the active projectile list
        activeProjectiles.push_back(projectile);
        activeProjectiles.back().id = static_cast<int>(activeProjectiles.size() - 1);

        AudioManager::PlaySFX(AudioManager::SFX::CannonFire);

        // Use selected ability
        abilityController->UseAbility();

        SetCamera(true); // Follow the projectile that is shot out of the cannon
        hasCannonShot = true;

        isPlayerControlled ? ++GameTrackingManager::data.playerShotsFired : ++GameTrackingManager::data.enemyShotsFired;

        std::cout << "[CannonManager] Projectile Fired!\n";
    }

    void Cannon::UpdateProjectile() {
        if (hasCannonShot) {
            allProjectileDestroyed = activeProjectiles.empty(); // check if all projectiles has been destroyed when cannon has been shot
        }
        if (activeProjectiles.empty()) return;

        for (Projectile& projectile : activeProjectiles) {
            if (!projectile.IsActive()) continue;

            float dt = PhysicsManager::g_dt;
            // Increased the frequency of the collision check by using loops
            // ensuring faster moving projectiles are check for collision multiple times
            // to ensure that the GHOSTING of block bug does not happen
            // Using sub_steps to ensure that it still moving at a per frame rate time (dt or 1/10 dt but 10 loops) even though it is looping
            const int SUB_STEPS = 10;
            float subDt = dt / SUB_STEPS;

            for (int step = 0; step < SUB_STEPS; ++step) {
                projectile.Update(isPlayerControlled, subDt);
                projectile.AbilityActivation(abilityController->CurrentAbility());
                AbilityActivation(projectile);
            }

            // Check if projectile need to be destroy and destroy after it lifespan
            projectile.Destroy();
            projectile.Draw();
        }

        RemoveInactiveProjectiles();
    }

    void Cannon::RemoveInactiveProjectiles() {
        // Clean up projectiles from the active projectiles list return if vector to destroy is empty
        if (activeProjectiles.empty()) return;
        for (int i = static_cast<int>(activeProjectiles.size() - 1); i >= 0; --i) {
            if (!activeProjectiles[i].IsActive()) {
                activeProjectiles[i].IsHittingSomething() ?
                    isPlayerControlled ? ++GameTrackingManager::data.playerShotsHit : ++GameTrackingManager::data.enemyShotsHit :
                    isPlayerControlled ? ++GameTrackingManager::data.playerShotsMiss : ++GameTrackingManager::data.enemyShotsMiss;

                // Erase from vector
                activeProjectiles.erase(activeProjectiles.begin() + i);
                std::cout << "[CannonManager] Remove projectile from vector. Active projectile vector count: " << activeProjectiles.size() << '\n';
            }
        }
    }

    void Cannon::AbilityActivation(Projectile& projectile) {
        if (abilityController->IsAbilityTriggered()) return;

        bool leftClick = AEInputCheckTriggered(AEVK_LBUTTON);

        switch (projectile.Mode()) {
        case ProjectileMode::ATTACK:
            // Assign stats boost here
            break;
        case ProjectileMode::AUTO_AIM:
            // Find new target if target is not alive
            if (!IsTargetAlive()) {
                SetTarget(LevelManager::CurrentLevel()->RandomCharacterObj(!isPlayerControlled)); // Randomly find a enemy target
            }

            // Calculate perfect shot angle and power
            PhysicsManager::PerfectShotCalculation(transform, target->transform, cannonAngle, shotPower);
            abilityController->IsAbilityTriggered(true);
            break;
        case ProjectileMode::CLUSTER_SHOT:
            if (!projectile.IsCollisionTriggered() && leftClick) {
                int count = 2;
                for (int i = 0; i < count; ++i) {
                    // Initialize and spawn a new projectile from current position
                    Projectile newProjectile = projectiles[selectedProjectileId];
                    float reducePercentage = i == 0 ? 0.90f : 1.10f;

                    newProjectile.Initialize(activeProjectiles.back().transform, static_cast<int>(activeProjectiles.size()),
                        { projectile.velocity.x * reducePercentage, projectile.velocity.y * reducePercentage });

                    // Add the these projectile to the active projectile list
                    activeProjectiles.push_back(newProjectile);
                    activeProjectiles.back().id = static_cast<int>(activeProjectiles.size() - 1);
                }

                abilityController->IsAbilityTriggered(true);
            }
            break;
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
        if (hasCannonShot) return; 
        int numDots = 30;
        // Starting position (at the barrel tip, matching ShootProjectile)
        float startX = transform.pos.x + cos(cannonAngle) * transform.size.x;
        float startY = transform.pos.y + sin(cannonAngle) * transform.size.x;

        // Initial velocity components
        float vx = cos(cannonAngle) * Config::CANNON_MAX_POWER;
        float vy = sin(cannonAngle) * Config::CANNON_MAX_POWER;

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

    void Cannon::DebugCommands() {
        if (!Utils::isDebuggerOn) return;

        // Toggle through all the ability
        if (AEInputCheckCurr(AEVK_LCTRL) || AEInputCheckCurr(AEVK_RCTRL)) {
            if (AEInputCheckTriggered(AEVK_X)) {
                abilityController->ToggleAbility(true, 1);
            }

            if (AEInputCheckTriggered(AEVK_Z)) {
                abilityController->ToggleAbility(true, -1);
            }
        }
        else {
            if (AEInputCheckTriggered(AEVK_X)) {
                abilityController->ToggleAbility(false, 1);
            }

            if (AEInputCheckTriggered(AEVK_Z)) {
                abilityController->ToggleAbility(false, -1);
            }
        }

        if (AEInputCheckTriggered(AEVK_Y)) {
            Utils::Wrap(++aiDifficulty, 0.0f, 1.0f);
            std::cout << "[CannonManager] AI Difficulty: " + std::to_string(aiDifficulty) << '\n';
        }
    }

    void Cannon::DebugInfo() {
        if (!Utils::isDebuggerOn) return;

        float mX = 0, mY = 0;
        Utils::GetWorldMousePos(mX, mY);
        float cX = transform.pos.x, cY = transform.pos.y;
        Utils::GetWorldPos(cX, cY);
        s32 mouseX, mouseY;
        AEInputGetCursorPosition(&mouseX, &mouseY);

        Projectile const& selectedProjectile = projectiles[selectedProjectileId];
        Ability* selectedAbility = abilityController->CurrentAbility();

        // Show cannon information
        std::string debugStr = "Cannon " + static_cast<std::string>(isPlayerControlled ? "PLAYER" : "ENEMY")
            + "\nIs Active : " + std::string(hasCannonShot ? "true" : "false")
            + "\nAll Projectile destroyed : " + static_cast<std::string>(allProjectileDestroyed ? "true" : "false")
            + "\nActive Projectiles Count: " + std::to_string(activeProjectiles.size())
            + "\nPos x: " + Utils::FormatFloat(transform.pos.x, 2) + " y: " + Utils::FormatFloat(transform.pos.y, 2)
            + "\nAngle: " + Utils::FormatFloat(cannonAngle * (180.0f / 3.14159265359), 2) + " " + Utils::FormatFloat(cannonAngle, 2)
            + "\n\nSelected Projectile\nId: " + std::to_string(selectedProjectile.ProjectileId())
            + "\nName: " + selectedProjectile.ProjectileName()
            + "\nMode: " + selectedProjectile.ModeName()
            + "\nDamage: " + std::to_string(selectedProjectile.attribute.Damage())
            + "\nRadius: " + std::to_string(selectedProjectile.attribute.DamageRadius())
            + "\n\nAbilities Inventory Count: " + std::to_string(abilityController->InventoryCount());

        for (Ability* const& ability : abilityController->Inventory()) {
            debugStr += "\nAbility: " + ability->GetName() + " Charges: " + std::to_string(ability->AbilityCharges());
        }

        debugStr += "\n\nAbility Modifiers:";

        for (Ability const& ability : abilityController->GetAbilityModifiers()) {
            debugStr += "\nAbility: " + ability.GetName() + " Turns: " + std::to_string(ability.GetStats().turns);
        }

        debugStr += "\n\nSelected Ability:\nId:" + std::to_string(selectedAbility->Id())
            + "\nName: " + selectedAbility->GetName()
            + "\nType: " + selectedAbility->GetTypeName()
            + "\nAbility Charges Remaining: " + std::to_string(selectedAbility->AbilityCharges())
            + "\nSelected Projectile Id: " + std::to_string(selectedAbility->GetProj().projectileId);

        if (target) {
            debugStr += "\nTarget: " + target->TypeName();
        }

        if (!isPlayerControlled) {
            debugStr += "\nAI Shoot Timer: " + Utils::FormatFloat(aiShootTimer, 2)
                + "\nAI Difficulty: " + std::to_string(aiDifficulty);
        }
        else {
            debugStr += "\n\nShortcuts:\n[CTRL + Z] < Toggle ALL Abilities > [CTRL + X]"
                "\n[Z] < Toggle Inventory Abilities > [X]";
        }

        // Print projectile that is shoot out information
        if (activeProjectiles.size() > 0) {
            debugStr += activeProjectiles.back().DebugInfo();
        }

        //UIManager::Panel panel{ {500,500}, {1,1,1,0.5f}, Utils::AnchorMode::TOP_RIGHT };
        //panel.Draw();

        debugText.TextContent(debugStr);
        debugText.Draw();
    }
}