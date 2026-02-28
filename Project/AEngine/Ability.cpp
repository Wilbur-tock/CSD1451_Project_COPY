#include "Ability.hpp"

Ability::Ability(int _id, std::string _name, AbilityType _type, int _projectileId, bool _unlimitedCharges)
	: id{ _id }, projectileId{ _projectileId }, name{ _name }, type{ _type }, charges{ 0 }, cost{ 0 }, unlimitedCharges{ _unlimitedCharges } {
}

int Ability::Id() const { return id; }
int Ability::ProjectileId() const { return projectileId; }
AbilityType Ability::Type() const { return type; }

int  Ability::Charges() const { return charges; }
void Ability::Charges(int toAdd) {
	charges += toAdd;
	charges = std::clamp(charges, 0, 100);
}

bool Ability::UnlimitedCharges() const { return unlimitedCharges; }