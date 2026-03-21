#ifndef CANNONMANAGER_H
#define CANNONMANAGER_H

#include "pch.hpp"

#include "AbilitiesManager.hpp"
#include "GameObject.hpp"
#include "Projectile.hpp"
#include "Character.hpp"
#include "UIManager.hpp"

namespace CannonManager {
	class Cannon : public GameObject {
	public: // Constructors
		Cannon() = default;
		Cannon(AEVec2 _pos, AEGfxTexture* _texture, TileIndex _tileId, AEVec2 _size = { 30,10 }, bool _isShootingRight = true, bool _isPlayerControlled = true);

		// Getters and Setters

		// return true if the projectile has been shoot out of the cannon and all projectiles has been destroyed
		bool AllProjectilesDestroyed() const { return allProjectileDestroyed; }
		AbilitiesManager::AbilityController* AbilityController() { return abilityController; }

		void SetTarget(Character* _target) { target = _target; }

		// Member Functions

		void Initialize(AbilitiesManager::AbilityController* controller);
		// Calculate the angle of the cannon based on the mouse position, render the cannon sprite
		// Calculate the charge shot power and update any shot projectiles
		// map is used to check for collision with the tiles
		void Update();
		void Draw();
		void DrawUI();
		void ResetCannon();

		// Return true if this cannon has a lock in target or the target has been destroyed
		bool IsTargetAlive();
		void SetEnemyAI(int difficultyLevel);

		void DebugInfo();
	private:
		// Initialize all the different type of projectile abilities to the CANNON CLASS
		// Switch between the projectileId to choose the different type of projectile abilities
		// 0 - Default projectile, 1 - Nuke Projectile , etc
		void ProjectileSettings();

		void EnemyAI();
		// Calculate the ideal cannonAngle and shotPower to hit the target,
		// add some variance to it to ensure the AI aim and shot feels "human"
		void EnemyAICalculation();

		// Check for left mouse input the shot power will be increased/decreased based on the charge rate
		void ChargeShot();
		void ClampAngle(float &angle);
		// Select the type of projectile ability based on the ability chosen by the player or ENEMY
		void SelectProjectileType();
		void ShootProjectile();
		// Render and move the projectile once it has reach the end of its lifespan it is destroy
		void UpdateProjectile();
		// Remove inactive projectiles from active projectiles vector
		void RemoveInactiveProjectiles();
		// Ability effects checker for ability to take effect before projectile has been shot 
		// Check for inputs to activated PROJECTILE Abilities after projectile has been shot
		// Ability effects that triggered on cannon
		void AbilityActivation(Projectile& projectile);
		void ApplyModifierEffects(Projectile& projectile);

		void SetCamera(bool isFollowingProjectile);
		void DrawTrajectory();

		void DebugCommands();
	private:
		bool isPlayerControlled = true, isShootingRight = true;
		bool hasCannonShot = false; // has projectile has been shot out of the cannon return true if yes
		bool isHoldingLeftClick = false; // player is holding left mouse click
		bool canPlayerShoot = true; // can player shoot used for checking if player press right click to cancel his shot
		bool allProjectileDestroyed = false; // projectile has been shot out and destroyed
		bool resetCamWhenShooting = false; // When player press left mouse click, reset camera to position the cannon and unable to move the camera or not

		AEVec2 defaultSize{ 30,10 };
		int projectileCount = 10;
		int projectileAbilitiesCount = 0;

		int selectedProjectileId = 0; // index of the selected projectile from the vector list of all the different type of projectiles
		std::vector<Projectile> projectiles; // vector containing all the different type of projectiles (Default, bomb , etc)
		std::vector<Projectile> activeProjectiles; // vector of all the active projectiles (Projectiles that have been shot out of the cannon)

		// Cannon attributes
		float cannonAngle = 0.0f; // Angle of the cannon for the projectile to shoot from
		// Shot power charging system (how far the cannon shoots the projectile)
		float chargingRate = Config::CANNON_MAX_POWER * 1.5f;
		float chargingPower = 0;
		float shotPower = 0; // The amount of shot power player charged up
		bool isPowerIncreasing = true;

		// Abilities variables
		AbilitiesManager::AbilityController* abilityController = nullptr;

		// Enemy AI variables
		float aiDifficulty = 0.0f; // 0 - 1 Range, 0 - easier, 1 - hardest
		float minAIShotDelay = 1.5f, maxAIShotDelay = 5.0f;
		float aiShootTimer = 0.0f; // randomise timer to visualise a delay for when the enemy when shoot out the prorjectile
		bool canAIShoot = false;
		Character* target = nullptr;

		//UI Object tied to this class
		UIManager::Bar screenOverlayShotBar, worldSpaceShotBar;
	};
}

#endif