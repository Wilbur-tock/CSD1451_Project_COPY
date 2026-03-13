#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Attributes.hpp"
#include "Tile.hpp"

struct ProjectileModule;
class Ability;

enum class ProjectileMode {
	IDLE		= 0,
	ATTACK		= 1,
	BOMB		= 2,
	NUKE		= 3,
	PENTA_BURST	= 4,
	AUTO_AIM	= 5,
	WALL		= 6,
};

class Projectile : public GameObject {
public:
	bool isGrounded = false;
	Attributes attribute;

	// Ctor

	Projectile() = default;
	Projectile(AEGfxTexture* _texture, ProjectileMode _mode = ProjectileMode::ATTACK, AEVec2 _pos = {0,0}, int _attackCharges = 1);

	// Accessors

	int  AttackCharges() const { return attackCharges; }
	void AttackCharges(int value) { attackCharges = std::clamp(value, 0, 100); }

	ProjectileMode Mode() const { return mode; }
	void Mode(ProjectileMode newMode) { mode = newMode; }

	int ProjectileId() const { return projectileId; }

	std::string ProjectileName() const { return projectileName; }

	inline std::string ModeName() const {
		switch (mode) {
		case ProjectileMode::IDLE:				return "IDLE"; break;
		case ProjectileMode::ATTACK:			return "DAMAGE"; break;
		case ProjectileMode::BOMB:				return "BOMB"; break;
		case ProjectileMode::NUKE:				return "NUKE"; break;
		case ProjectileMode::PENTA_BURST:		return "PENTA_BURST"; break;
		case ProjectileMode::AUTO_AIM:			return "AUTO_AIM"; break;
		case ProjectileMode::WALL:				return "WALL"; break;
		default:								return "YOU NEED TO ADD A PROJECTILE MODE NAME";
		}
	}

	bool IsActive() const { return isActive; }
	bool ToBeDestroyed() const { return toBeDestroyed; }
	bool IsCollisionTriggered() const { return isCollisionTriggered; }
	bool HasHitSomething() const { return hasHitSomething; }

	// Member functions

	void SetProjectileStats(std::string _projectileName, ProjectileModule const& data);

	void Initialize(Transform cannonTrans, int _activeId, float cannonAngle, float shotPower);
	void Render();
	// Ability effects checker for ability to take effect after projectile has been shot 
	// Check for inputs to activated PROJECTILE Abilities
	void AbilityActivation(Ability const& ability);
	void ProjectileMovement(float dt);
	// Simulate the projectile physics of bouncing off the walls, checking if it is grounded
	void OnCollision(Tile const& tile);
	// If the projectile hit something, REDUCE the velocity by a percentage for the FIRST COLLISION ONLY
	// So that it does not continue having the same projectile velocity even if it hits and destroy something
	void FirstTimeCollision();

	// Damage tiles based on projectile damage (Blocks & Enemy included)
	void DamageTile(int x, int y);
	void ConsumeAttackCharge();
	// Check if projectile is set to be destroyed
	void SetDestroy();
	// Return true if projectile has been destroyed otherwise false
	bool Destroy();

	std::string DebugInfo();

private:
	static constexpr AEVec2 defaultSize{ 10,10 };
	int projectileId = 0, attackCharges = 1;
	std::string projectileName;
	ProjectileMode mode = ProjectileMode::ATTACK;

	float timer = 0.0f; // lifespan timer
	bool isActive = false; // is projectile still alive or dead
	bool toBeDestroyed = false;
	bool isCollisionTriggered = false; // There is a collision triggered between the projectile and block
	bool isAbilityTriggered = false;
	bool hasHitSomething = false; // has hit any things expect for the ground / undestructible blocks
};

#endif // !PROJECTILE_HPP
