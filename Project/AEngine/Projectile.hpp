#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "pch.hpp"

#include "GameObject.hpp"
#include "Attributes.hpp"
#include "Tile.hpp"

struct ProjectileModule;
class Ability;

enum class ProjectileMode {
	ATTACK			= 0, 
	BOMB			= 1,
	NUKE			= 2,
	PENTA_BURST		= 3,
	AUTO_AIM		= 4,
	WALL			= 5,
	SHIELD_BUSTER	= 6,
	PIERCING		= 7,
	CLUSTER_SHOT	= 8,
	DECAY_SHOT		= 9,
	POISON			= 10,
	INSTANT_BASTION = 11,
	AIR_DROP		= 12,
	DRILL			= 13,
	TELEPORT			= 14,
	GOD_MODE        = 16,
};

class Projectile : public GameObject {
public:
	Attributes attribute;

	// Ctor

	Projectile() = default;
	Projectile(AEGfxTexture* _texture, ProjectileMode _mode = ProjectileMode::ATTACK, AEVec2 _pos = {0,0}, int _attackCharges = 1);

	// Accessors
public:
	int  AttackCharges() const { return attackCharges; }
	void AttackCharges(int value) { attackCharges = std::clamp(value, 0, 100); }

	ProjectileMode Mode() const { return mode; }
	void Mode(ProjectileMode newMode) { mode = newMode; }

	std::string ModeName() const {
		switch (mode) {
		case ProjectileMode::ATTACK:			return "DAMAGE";
		case ProjectileMode::BOMB:				return "BOMB";
		case ProjectileMode::NUKE:				return "NUKE";
		case ProjectileMode::PENTA_BURST:		return "PENTA_BURST";
		case ProjectileMode::AUTO_AIM:			return "AUTO_AIM";;
		case ProjectileMode::WALL:				return "WALL";
		case ProjectileMode::SHIELD_BUSTER:		return "SHIELD_BUSTER";
		case ProjectileMode::PIERCING:			return "PIERCING";
		case ProjectileMode::CLUSTER_SHOT:		return "CLUSTER_SHOT";
		case ProjectileMode::DECAY_SHOT:		return "DECAY_SHOT";
		case ProjectileMode::POISON:			return "POISON";
		case ProjectileMode::INSTANT_BASTION:	return "INSTANT_BASTION";
		case ProjectileMode::AIR_DROP:			return "AIR_DROP";
		case ProjectileMode::DRILL:				return "DRILL";
		case ProjectileMode::TELEPORT:			return "TELEPORT";
		default:								return "YOU NEED TO ADD A PROJECTILE MODE NAME";
		}
	}

	int ProjectileId() const { return projectileId; }
	std::string ProjectileName() const { return projectileName; }

	bool IsActive() const { return isActive; }
	bool HasHit() const { return hasHit; }
	bool HasCollided() const { return hasCollided; }

	void GravityScale(float newGravityScale) { gravityScale = newGravityScale; }

public:
	// Member functions

	void SetProjectileStats(std::string _projectileName, ProjectileModule const& data);
	void ApplyStatsBoost(ProjectileModule const& data);

	// Calculate the spawn position and starting velocity using angle and power
	void Initialize(Transform const& cannonTransform, int _activeId, float cannonAngle, float shotPower);
	void Initialize(Transform const& _transform, int _activeId, AEVec2 const& _velocity);
	void Update(bool isPlayerShot, float dt);
	void Draw();

	// Ability effects checker for ability to take effect after projectile has been shot 
	// Check for inputs to activated PROJECTILE Abilities
	void AbilityActivation(bool isPlayer, Ability const& ability);
	void ProjectileMovement(float dt);

	// Simulate the projectile physics of bouncing off the walls, checking if it is grounded
	void ConstantCollision(Tile const& tile);
	// If the projectile hit something, REDUCE the velocity by a percentage for the FIRST COLLISION ONLY
	// So that it does not continue having the same projectile velocity even if it hits and destroy something
	void FirstTimeCollision();

	// Damage tiles based on projectile damage (Blocks & Enemy included)
	void DamageTile(int x, int y);
	// Set a effect (Decaying tiles/poison characters) on a tile radius based on ability given
	void SetAreaEffects(Ability const& ability, bool isDecaying);

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

	void CanPhase(TileIndex const& center, int radius);
	void InPhasingTiles();

private:
	bool isGrounded = false;
	int projectileId = 0, attackCharges = 1;
	std::string projectileName;
	ProjectileMode mode = ProjectileMode::ATTACK;
	std::string modeName;

	float gravityScale = 1.0f;

	float timer = 0.0f; // lifespan timer

	// Phasing variables to ensure that projectile does not collide with any blocks that are newly build by abilities
	bool canPhase = false;
	bool isPhasing = false;
	bool prevDamageTileUndestructible = false; // does not keep checking if previous center damage tile is undestructible stop checking for destrucitble tiles

	bool isActive = false; // is projectile still alive or dead
	bool toBeDestroyed = false;
	bool hasCollided = false; // has hit any tile blocks that is not empty
	bool hasHit = false; // has hit any destructible tiles 

	bool isAbilityTriggered = false;
	bool hasHitShield = false;
	bool canAlwaysBypassBlocks = false;
	bool ignorePhysics = false;

	float aiLeftClickTimer = 0.0f;

	std::vector<TileIndex> tilesToPhase;
};

#endif // !PROJECTILE_HPP
