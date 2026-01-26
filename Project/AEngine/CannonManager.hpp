#ifndef CANNONMANAGER_H
#define CANNONMANAGER_H

#include "pch.hpp"
#include "GameObject.hpp"

namespace CannonManager {
	class Projectile : public GameObject {
	private:
		float projectileSize = 10;
		float gravity = 1500;

	public:
		float timer;
		AEVec2 velocity; // Vector rate of change
		bool isActive = false;

		Projectile(); // Default intializer
		Projectile(AEVec2 _pos, AEGfxTexture* _texture, AEVec2 _velocity);

		void Render();
		void MoveProjectile(float dt);
	};

	class Cannon : public GameObject {
	private:
		int ammoCount = 0;
		float projectileLifespan = 4;
		AEVec2 cannonSize{ 30,10 };

		// Power charging system (how far the cannon shoots the projectile)
		float power = 100, minPower = 100, maxPower = 2000, chargeRate = maxPower * 0.75f;
		bool isPowerIncrease = true;

	public:
		float cannonAngle; // Angle of the cannon for the projectile to shoot from
		std::vector<Projectile> ammo, activeAmmo;
		int ammoLimit;
		float shotPower; // The amount of shot power player charged up

		Cannon();
		Cannon(AEVec2 _pos, AEGfxTexture* _texture, int _projectileAmount);

		// Calculate the angle of the cannon based on the mouse position, render the cannon sprite
		// Calculate the charge shot power and update any shot projectiles
		void Update();
		// Check for left mouse input the shot power will be increased/decreased based on the charge rate
	    void ChargeShot(float dt);
		void Render();
		void ShootProjectile();
		// Render and move the projectile once it has reach the end of its lifespan it is destroy
		void ProjectileUpdate(float dt);
		void DestroyProjectile(Projectile& projectile);
		// UI to display the charging shot power bar */
		void UIChargeShotBar();
	};
}

#endif