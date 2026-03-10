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
	f32 hp, atk, def;
};

struct ProjectileModule {
	s32 projectileId;
	f32 damageAmp;
	s32 baseDamage;
	f32 radius;
	s32 charges;
	bool unlimited_charges;
};

struct DefenseModule {
	s32 shieldHP;
};

class Ability {
public:
	Ability(int _id, std::string _name, AbilityType _type, Tier _tier, std::string _texture);

	int					Id() const { return id; }
	std::string			GetName() const { return name; }
	AbilityType			GetType() const { return type; }
	Tier				GetTier() const { return tier; }
	std::string			GetTexture() const { return texture; }
						
	StatsModule			GetStats() const { return stats; }
	ProjectileModule	GetProj() const { return proj; }
	DefenseModule		GetDefense() const { return defense; }

	void SetStats(f32 _hp, f32 _atk, f32 _def);
	void SetProjectile(s32 _id, f32 _amp, s32 _basedamage, f32 _radius, s32 _charges, bool _unlimited);
	void SetDefense(s32 _shieldHP);
	void UpdateCharges(s32 _amount);

	int AbilityCharges() const;
	void AbilityCharges(int toAdd);
	int PreviousAbilityCharges() const;

private:
	int id; // To determind what type of ability it is (Bomb projectile, stats ability, etc)
	int abilityCharges = 0; // count of how many of these ability the player/enemy has
	int previousAbilityCharges = 0;
	std::string name;
	AbilityType type;
	Tier tier;
	std::string texture;

	StatsModule stats;
	ProjectileModule proj;
	DefenseModule defense;
};

std::ostream& operator<<(std::ostream& os, Ability ability);

#endif // !ABILITY_HPP
