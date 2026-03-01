#include "Ability.hpp"
#include "AbilitiesManager.hpp"

Ability::Ability(int _id, std::string _name, AbilityType _type, int _projectileId, bool _unlimitedCharges)
	: id{ _id }, projectileId{ _projectileId }, name{ _name }, type{ _type }, 
	previousCharges{0}, charges{ 0 }, cost{ 0 }, unlimitedCharges{ _unlimitedCharges } {
}

int Ability::Id() const { return id; }
std::string Ability::Name() const { return name; }
int Ability::ProjectileId() const { return projectileId; }
AbilityType Ability::Type() const { return type; }

int	 Ability::PreviousCharges() const { return previousCharges; }
int  Ability::Charges() const { return charges; }
void Ability::Charges(int toAdd) {
	previousCharges = charges;
	charges += toAdd;
	charges = std::clamp(charges, 0, 100);
}

bool Ability::UnlimitedCharges() const { return unlimitedCharges; }

std::ostream& operator<<(std::ostream& os, Ability ability) {
	os << ability.Name() << " Projectile Id: " << ability.ProjectileId() << " Remaining Charges: " << ability.Charges();
	return os;
}