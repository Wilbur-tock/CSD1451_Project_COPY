#ifndef ABILITY_HPP
#define ABILITY_HPP

#include "pch.hpp"

enum class AbilityType {
	STATISTICS = 1,
	OFFENSIVE = 2,
	DEFENSIVE = 3
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
	Ability(std::string _name, AbilityType _type, Tier _tier);
	//Ability(int _id, std::string _name, AbilityType _type, int _projectileId, bool _unlimitedCharges = false);


	std::string GetName() const;
	AbilityType GetType() const;
	Tier GetTier() const;

	StatsModule GetStats() const;
	ProjectileModule GetProj() const;
	DefenseModule GetDefense() const;

	void SetStats(f32 _hp, f32 _atk, f32 _def);
	void SetProjectile(s32 _id, f32 _amp, s32 _basedamage, f32 _radius, s32 _charges, bool _unlimited);
	void SetDefense(s32 _shieldHP);
	void UpdateCharges(s32 _amount);
	/*
	int Id() const;
	std::string Name() const;
	int ProjectileId() const;
	AbilityType Type() const;

	int PreviousCharges() const;
	int Charges() const;
	void Charges(int toAdd);

	bool UnlimitedCharges() const;
	*/
private:
	std::string name;
	AbilityType type;
	Tier tier;


	StatsModule stats;
	ProjectileModule proj;
	DefenseModule defense;

	//int id; // To determind what type of ability it is (Bomb projectile, stats ability, etc)
	//int projectileId; // To determined what type of projectiile ability it is
	//std::string name;
	//AbilityType type;
	//int previousCharges; // previous number of charges
	//int charges; // current number of charges
	//int cost;
	//bool unlimitedCharges = false;
};

std::ostream& operator<<(std::ostream& os, Ability ability);

#endif // !ABILITY_HPP
