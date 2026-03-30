/*
Authors: Dayrius, Wilbur
Description: Ability class which stores all the information about each ability such as
name, type, tier, etc.

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef ABILITY_HPP
#define ABILITY_HPP

#include "pch.hpp"

enum class AbilityType {
	STATS = 1,
	OFFENSIVE = 2,
	DEFENSIVE = 3,
	COUNT = DEFENSIVE, // ALWAYS THE LAST ONE
};

enum class Tier {
	BRONZE = 1,
	SLIVER = 2,
	GOLD = 3,
	LEGENDARY = 4
};

struct StatsModule {
	int hp = 0;
	int atk = 0;
	int shield = 0;
};

struct ProjectileModule {
	int projectileId = 0; // to determine which projectile mode the ability is using
	int damageMultiplier = 0;
	int baseDamage = 0;
	int radius = 0;
	int charges = 0; // projectile attack charges
	bool unlimitedCharges = 0;
};

struct StatusModule {
	bool isModifier = false; // is this ability a atk/hp stats boost or just a
	int rounds = 0; // number of rounds this ability would last
	bool isPositive = true;// is this ability a postive or negative effect (buff or debuff)
	bool targetSelf = true;  // stats buff/debuff on self or opponent
	float gravityScale = 1.0f;
	int damageEachRound = 0; // damage dealt over each round
	int effectRadius = 0;
};

class Ability {
public:
	Ability() = default;
	Ability(int _id, std::string _name, std::string _description, AbilityType _type, Tier _tier, AEGfxTexture* _texture);

	int					Id() const { return id; }
	void				Id(int newId) { id = newId; }
	std::string			GetName() const { return name; }
	std::string			GetDescription() const { return description; }
	AbilityType			GetType() const { return type; }
	Tier				GetTier() const { return tier; }
	AEGfxTexture*		GetTexture() const { return texture; }
	std::string	GetTypeName() const {
		switch (type) {
		case AbilityType::STATS:
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

	std::string GetDescriptionWithStats() const;

	float Chance() const { return chance; }
	// Increase or decrease chance of this ability appear 
	void Chance(float percentage) { chance = defaultChance * percentage; }
						
	StatsModule			GetStats() const { return stats; }
	ProjectileModule	GetProjectile() const { return projectile; }
	StatusModule        GetStatus() const { return status; }

	bool IsDefaultAbility() const { return id == 0 || name == "Default"; }

	void SetStats(StatsModule const& newStats) { stats = newStats; }
	void SetProjectile(ProjectileModule const& newProj) { projectile = newProj; }
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

	void ResetStats();
private:
	int id = 0; // To determind what type of ability it is (Bomb projectile, stats ability, etc)
	int abilityCharges = 0; // count of how many of these ability the player/enemy has
	int previousAbilityCharges = 0;
	std::string name = "";
	std::string description = "";
	AbilityType type;
	Tier tier;
	AEGfxTexture* texture = nullptr;

	// 0.0 - 1.0f chance to be choosen by ability selector
	float defaultChance = 0.0f;
	float chance = 0.0f; 
	// true if the ability caster ends the round after shooting, to ensure that the modifier they added
	// does not decrease by 1 round immediately
	bool justAddedModifier = false;

	StatsModule stats;
	ProjectileModule projectile;
	StatusModule status;
};

std::ostream& operator<<(std::ostream& os, Ability ability);

#endif // !ABILITY_HPP
