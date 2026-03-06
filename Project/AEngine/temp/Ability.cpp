#include "Ability.hpp"
#include "AbilitiesManager.hpp"
#include "json.hpp"
/*
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
*/
Ability::Ability(std::string _name, AbilityType _type, Tier _tier) : name(_name), type(_type), tier(_tier) { //inits all my stuff to 0(non-garbage value)
	stats = { 0.0f, 0.0f, 0.0f }; // HP, ATK, DEF
	proj = { 0, 0.0f, 0, false }; // ID, DmgAmp, Charges, IsUnlimitedCharge
	defense = { 0 }; // Shield HP
}

std::string Ability::GetName() const { return name; }
AbilityType Ability::GetType() const { return type; }
Tier Ability::GetTier() const { return tier; }

StatsModule Ability::GetStats() const { return stats; }
ProjectileModule Ability::GetProj() const { return proj; }
DefenseModule Ability::GetDefense() const { return defense; }

void Ability::UpdateCharges(s32 _amount) {
	// Only subtract if it's an offensive ability (since stats/defense might not use charges)
	if (type == AbilityType::OFFENSIVE) {

		// Access the variable inside the struct and modify it
		proj.charges += _amount;

		// Safety check: Don't let charges go below zero
		if (proj.charges < 0) {
			proj.charges = 0;
		}
	}
}

void Ability::SetStats(f32 _hp, f32 _atk, f32 _def) {
	stats.hp = _hp;
	stats.atk = _atk;
	stats.def = _def;
}

void Ability::SetProjectile(s32 _id, f32 _amp, s32 _basedamage, f32 _radius, s32 _charges, bool _unlimited) {
	proj.projectileId = _id;
	proj.damageAmp = _amp;
	proj.baseDamage = _basedamage;
	proj.radius = _radius;
	proj.charges = _charges;
	proj.unlimited_charges = _unlimited;
}

void Ability::SetDefense(s32 _shield) {
	defense.shieldHP = _shield;
}

std::ostream& operator<<(std::ostream& os, Ability ability) {
	os << "Ability Name: " << ability.GetName() << " Ability ID: " << ability.GetProj().projectileId 
		<< " Remaining Charges: " << ability.GetProj().charges;
	return os;
}
