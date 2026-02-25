#ifndef ABILITY_HPP
#define ABILITY_HPP

#include "pch.hpp"

enum class AbilityType {
	OFFENSIVE_PROJECTILE,
	OFFENSIVE,
	DEFENSIVE,
};

class Ability {
public:
	Ability(int _id, std::string _name, AbilityType _type, int _projectileId);

	int Id() const;
	int ProjectileId() const;
	AbilityType Type() const;

	int Charges() const;
	void Charges(int toAdd);

	bool UnlimitedCharges() const;
	void UnlimitedCharges(bool val);
private:
	int id; // To determind what type of ability it is (Bomb projectile, stats ability, etc)
	int projectileId; // To determined what type of projectiile ability it is
	std::string name;
	AbilityType type;
	int charges, cost;
	bool unlimitedCharges = false;
};

#endif // !ABILITY_HPP
