#ifndef ABILITIESFILEMANAGER_HPP
#define ABILITIESFILEMANAGER_HPP

#include "json.hpp"
#include "Ability.hpp"

namespace AbilitiesFileManager {
	void ReadAbilitiesFromFile(nlohmann::json& jsonData, std::string const& fileName);
	// Read all the basic information from the json data file, name, description, tier, etc
	Ability ReadAbilityInfo(int abilityId, nlohmann::json const& data);
	ProjectileModule ReadProjectileStats(nlohmann::json const& data);
	StatsModule ReadBaseStats(nlohmann::json const& data);
	StatusModule ReadStatusStats(nlohmann::json const& data);
}

#endif // !ABILITIESFILEMANAGER_HPP
