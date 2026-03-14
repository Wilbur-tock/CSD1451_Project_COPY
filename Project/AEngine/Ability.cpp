#include "Ability.hpp"
#include "AbilitiesManager.hpp"

Ability::Ability(int _id, std::string _name, AbilityType _type, Tier _tier, std::string _texture) 
	: id{_id}, name(_name), type(_type), tier(_tier), texture(_texture) { //inits all my stuff to 0(non-garbage value)
	stats = { 0.0f, 0.0f, 0.0f }; // HP, ATK, DEF
	proj = { 0, 0.0f, 0, 0.0f, 0, false }; // ID, DmgAmp, BaseDmg, Radius, Charges, IsUnlimitedCharge
	defense = { 0 }; // Shield HP
}

void Ability::SetStats(f32 _hp, f32 _atk, f32 _def, f32 _turns) {
	stats.hp = _hp;
	stats.atk = _atk;
	stats.def = _def;
	stats.turns = _turns;
}

void Ability::SetProjectile(s32 _id, f32 _amp, s32 _basedamage, f32 _radius, s32 _charges, bool _unlimited) {
	proj.projectileId = _id;
	proj.damageAmp = _amp;
	proj.baseDamage = _basedamage;
	proj.radius = _radius;
	proj.charges = _charges;
	proj.unlimitedCharges = _unlimited;
}

void Ability::SetDefense(s32 _shield) {
	defense.shieldHP = _shield;
}

int	 Ability::AbilityCharges() const { return abilityCharges; }
void Ability::AbilityCharges(int toAdd) {
	previousAbilityCharges = abilityCharges;
	abilityCharges += toAdd;
	abilityCharges = std::clamp(abilityCharges, 0, 100);
}
int Ability::PreviousAbilityCharges() const { return previousAbilityCharges; }

std::ostream& operator<<(std::ostream& os, Ability ability) {
	os << "Ability Name: " << ability.GetName() << " Ability ID: " << ability.GetProj().projectileId 
		<< " Remaining Charges: " << ability.GetProj().charges;
	return os;
}

