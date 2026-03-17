#ifndef ABILITY_HPP
#define ABILITY_HPP

#include "pch.hpp"

enum class AbilityType {
	STATISTICS = 1,
	OFFENSIVE = 2,
	DEFENSIVE = 3,
	COUNT = DEFENSIVE, // ALWAYS THE LAST ONE
};

enum class Tier {
	Bronze = 1,
	Silver = 2,
	Gold = 3,
	Legendary = 4
};

struct StatsModule {
	int hp = 0;
	int atk = 0;
	int shield = 0;
};

struct ProjectileModule {
	int projectileId = 0; // to determine which projectile mode the ability is using
	int damageAmp = 0;
	int baseDamage = 0;
	int radius = 0;
	int charges = 0; // projectile attack charges
	bool unlimitedCharges = 0;
};

struct DefenseModule {
	int shieldHP = 0;
};

struct StatusModule {
	bool isModifier = false; // is this ability a atk/hp stats boost or just a
	int rounds = 0; // number of rounds this ability would last
	bool isPositive = true;// is this ability a postive or negative effect (buff or debuff)
	bool targetSelf = true;  // stats buff/debuff on self or opponent
	float gravityScale = 0.0f;
	int damageEachRound = 0; // damage dealt over each round
	int effectRadius = 0;
};

class Ability {
public:
	Ability(int _id, std::string _name, AbilityType _type, Tier _tier, AEGfxTexture* _texture);

	int					Id() const { return id; }
	std::string			GetName() const { return name; }
	AbilityType			GetType() const { return type; }
	Tier				GetTier() const { return tier; }
	AEGfxTexture*		GetTexture() const { return texture; }
	std::string	GetTypeName() const {
		switch (type) {
		case AbilityType::STATISTICS:
			return "STATISTICS";
			break;
		case AbilityType::OFFENSIVE:
			return "OFFENSIVE";
			break;
		case AbilityType::DEFENSIVE:
			return "DEFENSIVE";
			break;
		}

		return "NULL ABILITY TYPE";
	}
						
	StatsModule			GetStats() const { return stats; }
	ProjectileModule	GetProj() const { return proj; }
	DefenseModule		GetDefense() const { return defense; }
	StatusModule        GetStatus() const { return status; }

	void SetStats(StatsModule const& newStats) { stats = newStats; }
	void SetProjectile(ProjectileModule const& newProj) { proj = newProj; }
	void SetDefense(DefenseModule const& newDef) { defense = newDef; }
	void SetStatus(StatusModule const& newStatus) { status = newStatus; }

	int	 AbilityCharges() const { return abilityCharges; }
	void AbilityCharges(int toAdd) {
		previousAbilityCharges = abilityCharges;
		abilityCharges += toAdd;
		abilityCharges = std::clamp(abilityCharges, 0, 100);
	}
	int PreviousAbilityCharges() const { return previousAbilityCharges; }

	void UpdateRoundLifespan() { 
		if (justAddedModifier) {
			justAddedModifier = false;
			return;
		}
		--status.rounds;
	}

	void SetShield(int newValue) { stats.shield = newValue; }

	// return true if is an ability modifier meaning the ability effects is applied for more than 1 round
	bool IsAbilityModifier() const { return status.isModifier && status.rounds > 0; }

	void JustAddModifier(bool justAdd) { justAddedModifier = justAdd; }
	bool JustAddModifier() const { return justAddedModifier; }
private:
	int id; // To determind what type of ability it is (Bomb projectile, stats ability, etc)
	int abilityCharges = 0; // count of how many of these ability the player/enemy has
	int previousAbilityCharges = 0;
	std::string name = "";
	AbilityType type;
	Tier tier;
	AEGfxTexture* texture;

	bool justAddedModifier = false;

	StatsModule stats;
	ProjectileModule proj;
	DefenseModule defense;
	StatusModule status;
};

std::ostream& operator<<(std::ostream& os, Ability ability);

#endif // !ABILITY_HPP
