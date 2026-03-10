#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Attributes.hpp"
#include "Tile.hpp"
#include "AbilitiesManager.hpp"

struct ProjectileModule;

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
	float timer = 0.0f;
	bool isActive = false, isGrounded = false, toBeDestroyed = false;
	bool collisionTriggered = false; // There is a collision triggered between the projectile and block

	Attributes attribute;

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

	// Member functions

	void SetProjectileStats(std::string _projectileName, ProjectileModule const& data);

	void Initialize(Transform cannonTrans, int _activeId, float cannonAngle, float shotPower);
	void Render();
	// Ability effects checker for ability to take effect after projectile has been shot 
	// Check for inputs to activated PROJECTILE Abilities
	void AbilityActivation();
	void MoveProjectile(float dt);
	// Simulate the projectile physics of bouncing off the walls, checking if it is grounded
	void OnCollision(Tile const& tile);
	// Damage tiles based on projectile damage (Blocks & Enemy included)
	void DamageTile(int x, int y);
	void ConsumeAttackCharge();
	void Destroy();

private:
	static constexpr AEVec2 defaultSize{ 10,10 };
	int projectileId = 0, attackCharges = 1;
	std::string projectileName;
	ProjectileMode mode = ProjectileMode::ATTACK;
	bool abilityTriggered = false;
};

#endif // !PROJECTILE_HPP
