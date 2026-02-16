#ifndef CANNONMANAGER_H
#define CANNONMANAGER_H

#include "pch.hpp"

#include "GameObject.hpp"

namespace CannonManager {
	enum ProjectileMode {
		IDLE,
		ATTACK,
	};

	class Projectile : public GameObject {
	private:
		AEVec2 defaultSize{ 10,10 };
		int attackCharges = 1;
		ProjectileMode mode = IDLE;
	public:
		float timer = 0.0f;
		AEVec2 velocity = { 0,0 }; // Vector rate of change of displacement (direction * speed)
		bool isActive = false, isCollisionDetected = false, isGrounded = false, toBeDestroyed = false;

		Projectile();
		Projectile(AEVec2 _pos, AEGfxTexture* _texture, AEVec2 _velocity);

		int AttackCharges() const;
		void AttackCharges(int value);
		ProjectileMode Mode() const;
		void Mode(ProjectileMode newMode);

		void Render();
		void MoveProjectile(float dt);
		void DamageBlocks(int x, int y, int damage, int damageRadiusMultipler);
		void ConsumeAttackCharge();
		void Destroy();
	};

	class Cannon : public GameObject {
	private:
		int ammoCount = 0, ammoLimit = 10;
		bool isShootingRight = true;
		bool isActive = false; // has projectile has been shot out of the cannon
		float cannonAngle = 0.0f; // Angle of the cannon for the projectile to shoot from

		float projectileLifespan = 2.0f;
		int defaultDamage{10}, minDamage{0},maxDamage{100};
		int damage = 100; // Damage of the projectile shot from the cannon
		int damageTileRadiusMultipler = 2;
		AEVec2 defaultSize{ 30,10 };

		// Power charging system (how far the cannon shoots the projectile)
		float minPower = 100, maxPower = 2500, chargeRate = maxPower * 0.75f;
		float chargingPower;
		float shotPower; // The amount of shot power player charged up
		bool isPowerIncrease = true;
	public:
		std::vector<Projectile> ammo, activeAmmo;

		// Constructors
		Cannon();
		Cannon(AEVec2 _pos, AEGfxTexture* _texture, int _ammoLimit, bool _isShootingRight = true, AEVec2 _size = {30,10});

		// Getters & Setters
		
		int Damage() const;
		void Damage(int _damage);

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