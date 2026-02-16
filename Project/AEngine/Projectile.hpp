#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Attributes.hpp"

enum ProjectileMode {
	IDLE,
	ATTACK,
	BOMB,
};

class Projectile : public GameObject {
private:
	static constexpr AEVec2 defaultSize{ 10,10 };
	int projectileId = 0, attackCharges = 1;
	ProjectileMode mode = ATTACK;
public:
	AEVec2 velocity = { 0,0 }; // Vector rate of change of displacement (direction * speed)
	float timer = 0.0f;
	bool isActive = false, isCollisionDetected = false, isGrounded = false, toBeDestroyed = false;

	Attributes attribute;

	Projectile();
	Projectile(AEGfxTexture* _texture, ProjectileMode _mode = ATTACK, AEVec2 _pos = {0,0}, int _attackCharges = 1, AEVec2 _velocity = {0,0});

	// Accessors

	int AttackCharges() const;
	void AttackCharges(int value);

	ProjectileMode Mode() const;
	void Mode(ProjectileMode newMode);

	int ProjectileId() const;

	// Member functions
	void AbilityUpdate();
	void Render();
	void MoveProjectile(float dt);
	void DamageBlocks(int x, int y);
	void ConsumeAttackCharge();
	void Destroy();
};

#endif // !PROJECTILE_HPP
