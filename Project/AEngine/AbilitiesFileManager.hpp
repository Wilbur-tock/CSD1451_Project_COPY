#ifndef ABILITIESFILEMANAGER_HPP
#define ABILITIESFILEMANAGER_HPP

#include "json.hpp"
#include "Ability.hpp"

namespace AbilitiesFileManager {
	void ReadJsonFile(nlohmann::json& jsonData, std::string const& fileName);
	// Read all the basic information from the json data file, name, description, tier, etc
	Ability ReadAbilityInfo(nlohmann::json const& data);
	// Read all the keys of the different stats and assigned the values to the Ability class
	Ability& ReadAllStats(Ability& ability, nlohmann::json const& data);

	ProjectileModule ReadProjectileStats(nlohmann::json const& data);
	StatsModule ReadBaseStats(nlohmann::json const& data);
	StatusModule ReadStatusStats(nlohmann::json const& data);
}

#endif // !ABILITIESFILEMANAGER_HPP
