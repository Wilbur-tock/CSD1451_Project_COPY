#include "Ability.hpp"
#include "AbilitiesManager.hpp"

Ability::Ability(int _id, std::string _name, std::string _description, AbilityType _type, Tier _tier, AEGfxTexture* _texture)
	: id{ _id }, name(_name), description{_description}, type(_type), tier(_tier), texture(_texture) {}

std::ostream& operator<<(std::ostream& os, Ability ability) {
	os << "Ability Name: " << ability.GetName() << " ID: " << ability.GetProj().projectileId 
		<< " Remaining Charges: " << ability.GetProj().charges;
	return os;
}

