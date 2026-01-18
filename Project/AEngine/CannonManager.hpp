#ifndef CANNONMANAGER_H
#define CANNONMANAGER_H

#include "pch.hpp"

namespace CannonManager {
	class Projectile {
	private:
		float projectileSize = 10;
		float gravity = 1500;

	public:
		float timer;
		AEVec2 pos, velocity;
		bool isActive = false;

		Projectile();
		Projectile(AEVec2 _pos, AEVec2 _velocity);

		void Render();
		void MoveProjectile(float speed, float dt);
		void DestroyProjectile();
	};

	class Cannon {
	private:
		int projectileUsed = 0, projectileLifespan = 10;
		AEVec2 cannonSize{ 30,10 };

		// Power charging system (how far the cannon shoots the projectile)
		float power = 100, minPower = 100, maxPower = 1500, chargeRate = 500;
		bool isPowerIncrease = true;

	public:
		AEVec2 pos;
		float cannonAngle;
		std::vector<Projectile> bullets, activeProjectile;
		int projectileAmount;
		float projectileSpeed;

		Cannon();
		Cannon(AEVec2 _pos, int _projectileAmount);

		void Update();
	    void ChargeShot(float dt);
		void Render();
		void ShootProjectile(float dt);
		void ProjectileUpdate(float dt);
		void UI();
	};
}

#endif