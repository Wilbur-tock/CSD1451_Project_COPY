#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Attributes.hpp"
#include "Tile.hpp"

enum ProjectileMode {
	IDLE,
	ATTACK,
	BOMB,
	FAST,
};

class Projectile : public GameObject {
private:
	static constexpr AEVec2 defaultSize{ 10,10 };
	int projectileId = 0, attackCharges = 1;
	ProjectileMode mode = ATTACK;
public:
	float timer = 0.0f;
	bool isActive = false, isGrounded = false, toBeDestroyed = false;
	bool isCollisionDetected = false; // collision is detected for the FIRST TIME
	bool hasCollision = false; // collision has been detected in the past frame

	Attributes attribute;

	Projectile();
	Projectile(AEGfxTexture* _texture, ProjectileMode _mode = ATTACK, AEVec2 _pos = {0,0}, int _attackCharges = 1);

	// Accessors

	int AttackCharges() const;
	void AttackCharges(int value);

	ProjectileMode Mode() const;
	void Mode(ProjectileMode newMode);

	int ProjectileId() const;

	// Member functions
	void AbilityUpdate();
	void Render();
	// Simulate the projectile physics of bouncing off the walls, checking if it is grounded
	void SimulatePhysics(Tile const& tile);
	void MoveProjectile(float dt);
	// Damage tiles based on projectile damage (Blocks & Enemy included)
	void DamageTile(int x, int y);
	void ConsumeAttackCharge();
	void Destroy();
};

#endif // !PROJECTILE_HPP
