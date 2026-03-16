#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Attributes.hpp"
#include "Tile.hpp"

struct ProjectileModule;
class Ability;

enum class ProjectileMode {
	IDLE			= 0,
	ATTACK			= 1,
	BOMB			= 2,
	NUKE			= 3,
	PENTA_BURST		= 4,
	AUTO_AIM		= 5,
	WALL			= 6,
	SHIELD_BUSTER	= 7,
	PIERCING		= 8,
	CLUSTER_SHOT	= 9,
};

class Projectile : public GameObject {
public:
	Attributes attribute;

	// Ctor

	Projectile() = default;
	Projectile(AEGfxTexture* _texture, ProjectileMode _mode = ProjectileMode::ATTACK, AEVec2 _pos = {0,0}, int _attackCharges = 1);

	// Accessors

	int  AttackCharges() const { return attackCharges; }
	void AttackCharges(int value) { attackCharges = std::clamp(value, 0, 100); }

	ProjectileMode Mode() const { return mode; }
	void Mode(ProjectileMode newMode) { mode = newMode; }

	std::string ModeName() const {
		switch (mode) {
		case ProjectileMode::IDLE:				return "IDLE";
		case ProjectileMode::ATTACK:			return "DAMAGE";
		case ProjectileMode::BOMB:				return "BOMB";
		case ProjectileMode::NUKE:				return "NUKE";
		case ProjectileMode::PENTA_BURST:		return "PENTA_BURST";
		case ProjectileMode::AUTO_AIM:			return "AUTO_AIM";;
		case ProjectileMode::WALL:				return "WALL";
		case ProjectileMode::SHIELD_BUSTER:		return "SHIELD_BUSTER";
		case ProjectileMode::PIERCING:			return "PIERCING";
		case ProjectileMode::CLUSTER_SHOT:		return "CLUSTER_SHOT";
		default:								return "YOU NEED TO ADD A PROJECTILE MODE NAME";
		}
	}

	int ProjectileId() const { return projectileId; }

	std::string ProjectileName() const { return projectileName; }

	bool IsActive() const { return isActive; }
	bool IsHittingSomething() const { return isHitSomething; }
	bool IsCollisionTriggered() const { return isCollisionTriggered; }

	// Member functions

	void SetProjectileStats(std::string _projectileName, ProjectileModule const& data);

	// Calculate the spawn position and starting velocity using angle and power
	void Initialize(Transform const& cannonTransform, int _activeId, float cannonAngle, float shotPower);
	void Initialize(Transform const& _transform, int _activeId, AEVec2 const& _velocity);
	void Update(bool isPlayerShot, float dt);
	void Draw();

	// Ability effects checker for ability to take effect after projectile has been shot 
	// Check for inputs to activated PROJECTILE Abilities
	void AbilityActivation(Ability const* ability);
	void ProjectileMovement(float dt);

	// Simulate the projectile physics of bouncing off the walls, checking if it is grounded
	void ConstantCollision(Tile const& tile);
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
	// Instantly destroy the projectile after the timer runs out or
	// it has hit a shield or some blocks that instant destroy projectiles
	void InstantDestroy();
	void PlayModeEffect();

private:
	bool isGrounded = false;
	int projectileId = 0, attackCharges = 1;
	std::string projectileName;
	ProjectileMode mode = ProjectileMode::ATTACK;
	std::string modeName;

	float timer = 0.0f; // lifespan timer
	bool isActive = false; // is projectile still alive or dead
	bool toBeDestroyed = false;
	bool isCollisionTriggered = false; // There is a collision triggered between the projectile and block for the first time
	bool isAbilityTriggered = false;

	bool isHitSomething = false; // has hit any things expect for the ground / undestructible blocks
	bool hasHitShield = false;
	bool canBypassBlocks = false;
};

#endif // !PROJECTILE_HPP
