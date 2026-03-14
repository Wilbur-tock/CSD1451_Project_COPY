#include "Ability.hpp"
#include "AbilitiesManager.hpp"

Ability::Ability(int _id, std::string _name, AbilityType _type, Tier _tier, std::string _texture) 
	: id{_id}, name(_name), type(_type), tier(_tier), texture(_texture) { //inits all my stuff to 0(non-garbage value)
	stats = { 0, 0, 0, 0 }; // HP, ATK, DEF
	proj = { 0, 0, 0, 0, 0, false }; // ID, DmgAmp, BaseDmg, Radius, Charges, IsUnlimitedCharge
	defense = { 0 }; // Shield HP
}

std::ostream& operator<<(std::ostream& os, Ability ability) {
	os << "Ability Name: " << ability.GetName() << " Ability ID: " << ability.GetProj().projectileId 
		<< " Remaining Charges: " << ability.GetProj().charges;
	return os;
}

