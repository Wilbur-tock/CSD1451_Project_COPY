#ifndef CANNONMANAGER_H
#define CANNONMANAGER_H

#include "pch.hpp"

#include "GameObject.hpp"
#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Color.hpp"
#include "CameraManager.hpp"
#include "GameStateManager.hpp"
#include "Utils.hpp"
#include "CollisionManager.hpp"

namespace CannonManager {
	enum ProjectileMode {
		IDLE,
		ATTACK,
	};

	class Projectile : public GameObject {
	private:
		AEVec2 defaultSize{ 10,10 };
		float gravity = 1500;

	public:
		float timer = 0.0f;
		int numOfAttacks = 1;
		AEVec2 velocity; // Vector rate of change
		bool isActive = false, collisionDetected = false, isGrounded = false, toBeDestroyed = false;
		ProjectileMode mode = IDLE;

		Projectile(); // Default intializer
		Projectile(AEVec2 _pos, AEGfxTexture* _texture, AEVec2 _velocity);

		void Render();
		void MoveProjectile();
		void Destroy();
	};

	class Cannon : public GameObject {
	private:
		int ammoCount = 0, ammoLimit;
		float cannonAngle; // Angle of the cannon for the projectile to shoot from
		float projectileLifespan = 4, defaultDamage{ 10.0f }, minDamage{ 0.0f },maxDamage{ 100.0f };
		float damage = defaultDamage; // Damage of the projectile shot from the cannon
		AEVec2 defaultSize{ 30,10 };

		// Power charging system (how far the cannon shoots the projectile)
		float power = 100, minPower = 100, maxPower = 2500, chargeRate = maxPower * 0.75f;
		float shotPower; // The amount of shot power player charged up
		bool isPowerIncrease = true;

	public:
		std::vector<Projectile> ammo, activeAmmo;

		// Intializer

		Cannon();
		Cannon(AEVec2 _pos, AEGfxTexture* _texture, int _projectileAmount);
		Cannon(AEVec2 _pos, AEGfxTexture* _texture, int _projectileAmount, AEVec2 _size);

		// Getters & Setters
		
		float GetDamage() const;
		void SetDamage(float _damage);

		// Reset values of the cannon to be 0
		void Reset();
		// Calculate the angle of the cannon based on the mouse position, render the cannon sprite
		// Calculate the charge shot power and update any shot projectiles
		// map is used to check for collision with the tiles
		void Update();
		// Check for left mouse input the shot power will be increased/decreased based on the charge rate
		void ChargeShot();
		void Render();
		void ShootProjectile();
		// Render and move the projectile once it has reach the end of its lifespan it is destroy
		void UpdateProjectile();
		// Destroy projectile once it reached the end of its lifespan or out of the map bounds
		void DestroyProjectile(Projectile& projectile);
		// UI to display the charging shot power bar */
		void UIChargeShotBar();
		void SetCamera(bool isFollowingProjectile);
	};
}

#endif