#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Attributes.hpp"
#include "Tile.hpp"
#include "AbilitiesManager.hpp"

struct ProjectileModule;

enum class ProjectileMode {
	IDLE,
	ATTACK,
	BOMB,
	NUKE,
};

class Projectile : public GameObject {
public:
	float timer = 0.0f;
	bool isActive = false, isGrounded = false, toBeDestroyed = false;
	bool collisionTriggered = false; // collision is detected for the FIRST TIME

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
		case ProjectileMode::IDLE:		return "IDLE"; break;
		case ProjectileMode::ATTACK:	return "DAMAGE"; break;
		case ProjectileMode::BOMB:		return "BOMB"; break;
		case ProjectileMode::NUKE:		return "NUKE"; break;
		default:						return "YOU NEED TO ADD A PROJECTILE MODE NAME";
		}
	}

	// Member functions

	void SetProjectileStats(std::string _projectileName, ProjectileModule const& data);

	void Render();
	// Check for inputs to activated PROJECTILE Abilities
	void AbilityUpdate();
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
};

#endif // !PROJECTILE_HPP
