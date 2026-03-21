#include "Ability.hpp"
//#include "AbilitiesManager.hpp"

std::vector<float> tierChances{
	0.5f,
	0.25f,
	0.1f,
	0.05f,
};


Ability::Ability(int _id, std::string _name, std::string _description, AbilityType _type, Tier _tier, AEGfxTexture* _texture)
	: id{ _id }, name(_name), description{ _description }, type(_type), tier(_tier), texture(_texture), 
	defaultChance{ tierChances[static_cast<int>(_tier) - 1] }, chance{ defaultChance } {}

void Ability::ResetStats() {
	SetStats(StatsModule{});
	SetProjectile(ProjectileModule{});
	SetStatus(StatusModule{});
}

std::ostream& operator<<(std::ostream& os, Ability ability) {
	os << "Ability Name: " << ability.GetName() << " ID: " << ability.GetProjectile().projectileId 
		<< " Remaining Charges: " << ability.GetProjectile().charges;
	return os;
}

