#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "pch.hpp"

#include "GameObject.hpp"

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
	AEVec2 velocity = { 0,0 }; // Vector rate of change of displacement (direction * speed)
	float timer = 0.0f;
	bool isActive = false, isCollisionDetected = false, isGrounded = false, toBeDestroyed = false;

	Projectile();
	Projectile(AEVec2 _pos, AEGfxTexture* _texture, AEVec2 _velocity);

	// Accessors

	int AttackCharges() const;
	void AttackCharges(int value);

	ProjectileMode Mode() const;
	void Mode(ProjectileMode newMode);

	// Member functions

	void Render();
	void MoveProjectile(float dt);
	void DamageBlocks(int x, int y, int damage, int blastRadiusMod);
	void ConsumeAttackCharge();
	void Destroy();
};

#endif // !PROJECTILE_HPP
