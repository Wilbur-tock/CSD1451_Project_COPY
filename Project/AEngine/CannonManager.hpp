#ifndef CANNONMANAGER_H
#define CANNONMANAGER_H

#include "pch.hpp"

#include "AbilitiesManager.hpp"
#include "GameObject.hpp"
#include "Projectile.hpp"

namespace CannonManager {
	class Cannon : public GameObject {
	public:
		// Constructors
		Cannon() = default;
		Cannon(AEVec2 _pos, AEGfxTexture* _texture, TileIndex _tileId, AEVec2 _size = { 30,10 }, bool _isShootingRight = true, bool _isPlayerControlled = true);

		// Getters & Setters
		void SetAbilityController(AbilitiesManager::AbilityController* controller);
		// return true if projectile has been shoot out of the cannon and be destroyed
		bool HasShot() const;

		// Member Functions

		// Reset values of the cannon ammo vectors to be in its default state
		void ProjectileSettings();
		// Calculate the angle of the cannon based on the mouse position, render the cannon sprite
		// Calculate the charge shot power and update any shot projectiles
		// map is used to check for collision with the tiles
		void Update();
		void Render();
		void ResetTurn();

		void EnemyAI(AEVec2 const targetPos);
		// Calculate the ideal cannonAngle and shotPower to hit the target,
		// add some variance to it to ensure the AI aim and shot feels "human"
		void EnemyAICalculation(AEVec2 const targetPos);

		// Check for left mouse input the shot power will be increased/decreased based on the charge rate
		void ChargeShot();
		void ClampAngle(float &angle);
		// Select the type of projectile ability based on the ability chosen by the player or ENEMY
		void SelectProjectile();
		void ShootProjectile();
		// Render and move the projectile once it has reach the end of its lifespan it is destroy
		void UpdateProjectile();
		// Destroy projectile once it reached the end of its lifespan or out of the map bounds
		void DestroyProjectile(Projectile& projectile);
		// UI to display the charging shot power bar */

		void SetCamera(bool isFollowingProjectile);
		void DrawTrajectory();
	private:
		bool isPlayerControlled = true, isShootingRight = true;
		bool isActive = false; // has projectile has been shot out of the cannon return true if yes
		float cannonAngle = 0.0f; // Angle of the cannon for the projectile to shoot from
		bool hasShot = false; // projectile has been shot out and destroyed
		bool resetCamWhenShooting = false; // When player press left mouse click, reset camera to position the cannon and unable to move the camera or not

		// Cannon attributes
		AEVec2 defaultSize{ 30,10 };
		int projectileCount = 10;
		int selectedProjectileId = 0; // index of the selected projectile from the vector list of all the different type of projectiles
		std::vector<Projectile> projectiles; // vector containing all the different type of projectiles (Default, bomb , etc)
		std::vector<Projectile> activeProjectiles; // vector of all the active projectiles (Projectiles that have been shot out of the cannon)

		// Shot power charging system (how far the cannon shoots the projectile)
		float minPower = 100, maxPower = 2500, chargeRate = maxPower * 0.75f;
		float chargingPower = 0;
		float shotPower = 0; // The amount of shot power player charged up
		bool isPowerIncreasing = true;

		AbilitiesManager::AbilityController* abilityController = nullptr;

		// Enemy AI variables
		float aiShootTimer = 0.0f; // randomise timer to visualise a delay for when the enemy when shoot out the prorjectile
		bool canAIShoot = false;
	};
}

#endif