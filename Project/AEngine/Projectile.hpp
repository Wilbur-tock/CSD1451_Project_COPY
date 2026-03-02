#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Attributes.hpp"
#include "Tile.hpp"

enum class ProjectileMode {
	IDLE,
	ATTACK,
	BOMB,
	FAST,
};

class Projectile : public GameObject {
public:
	float timer = 0.0f;
	bool isActive = false, isGrounded = false, toBeDestroyed = false;
	bool collisionTriggered = false; // collision is detected for the FIRST TIME
	bool isCollisionActive = false; // collision has been detected in the past frame

	Attributes attribute;

	Projectile() = default;
	Projectile(AEGfxTexture* _texture, ProjectileMode _mode = ProjectileMode::ATTACK, AEVec2 _pos = {0,0}, int _attackCharges = 1);

	// Accessors

	int AttackCharges() const;
	void AttackCharges(int value);

	ProjectileMode Mode() const;
	void Mode(ProjectileMode newMode);
	inline std::string ModeName() const {
		switch (mode) {
		case ProjectileMode::IDLE:		return "IDLE";
		case ProjectileMode::ATTACK:	return "Attack Projectile";
		case ProjectileMode::BOMB:		return "Bomb Projectile";
		case ProjectileMode::FAST:		return "Fast Projectile";
		default:						return "YOU NEED TO ADD A PROJECTILE NAME";
		}
	}

	int ProjectileId() const;

	// Member functions

	void Render();

	void AbilityUpdate();
	void MoveProjectile(float dt);
	// Simulate the projectile physics of bouncing off the walls, checking if it is grounded
	void SimulatePhysics(Tile const& tile);
	// Damage tiles based on projectile damage (Blocks & Enemy included)
	void DamageTile(int x, int y);
	void ConsumeAttackCharge();
	void Destroy();

private:
	static constexpr AEVec2 defaultSize{ 10,10 };
	int projectileId = 0, attackCharges = 1;
	ProjectileMode mode = ProjectileMode::ATTACK;
};

#endif // !PROJECTILE_HPP
