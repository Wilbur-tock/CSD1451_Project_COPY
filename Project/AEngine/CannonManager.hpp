#ifndef CANNONMANAGER_H
#define CANNONMANAGER_H

#include "pch.hpp"

#include "GameObject.hpp"
#include "Projectile.hpp"

namespace CannonManager {
	class Cannon : public GameObject {
	private:
		bool isShootingRight = true;
		bool isActive = false; // has projectile has been shot out of the cannon
		float cannonAngle = 0.0f; // Angle of the cannon for the projectile to shoot from

		// Cannon attributes
		AEVec2 defaultSize{ 30,10 };
		float projectileLifespan = 2.0f;
		int selectedProjectileId = 1; // selected projectile index
		int ammoCount = 10;
		std::vector<Projectile> ammo;

		// Power charging system (how far the cannon shoots the projectile)
		float minPower = 100, maxPower = 2500, chargeRate = maxPower * 0.75f;
		float chargingPower = 0;
		float shotPower = 0; // The amount of shot power player charged up
		bool isPowerIncrease = true;
	public:
		std::vector<Projectile> activeAmmo;

		// Constructors
		Cannon();
		Cannon(AEVec2 _pos, AEGfxTexture* _texture, bool _isShootingRight = true, AEVec2 _size = {30,10});

		// Getters & Setters

		// Reset values of the cannon ammo vectors to be in its default state
		void Reset();
		// Calculate the angle of the cannon based on the mouse position, render the cannon sprite
		// Calculate the charge shot power and update any shot projectiles
		// map is used to check for collision with the tiles
		void Update();
		void Render();

		// Check for left mouse input the shot power will be increased/decreased based on the charge rate
		void ChargeShot();

		void ShootProjectile();
		// Render and move the projectile once it has reach the end of its lifespan it is destroy
		void UpdateProjectile();
		// Destroy projectile once it reached the end of its lifespan or out of the map bounds
		void DestroyProjectile(Projectile& projectile);
		// UI to display the charging shot power bar */

		void UIChargeShotBar();
		void SetCamera(bool isFollowingProjectile);
		void DrawTrajectory();
	};
}

#endif