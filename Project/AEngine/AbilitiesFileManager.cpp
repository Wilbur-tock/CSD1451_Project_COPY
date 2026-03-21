#include "AbilitiesFileManager.hpp"

#include "AssetsManager.hpp"

namespace AbilitiesFileManager {
	std::string filePath = "Assets/DataAbilities/"; // if filepath name is changed only need to change here
	std::string statsKey = "Base_Stats";
	std::string projectileKey = "Projectile_Stats";
	std::string statusKey = "Status_Stats";

	void ReadAbilitiesFromFile(nlohmann::json& jsonData, std::string const& fileName) {
		std::string filePathAndName = filePath + fileName;
		// Open the file
		std::ifstream file(filePathAndName);
		if (!file.is_open()) {
			std::cerr << "[AbilitiesManager] ERROR: Could not open " << filePathAndName << '\n';
			return;
		}

		nlohmann::json newData;
		file >> newData;

		if (jsonData.is_null()) {
			jsonData = newData; // First read, just assign
		}
		else if (jsonData.is_array() && newData.is_array()) {
			jsonData.insert(jsonData.end(), newData.begin(), newData.end()); // Merge arrays
		}
		else if (jsonData.is_object() && newData.is_object()) {
			jsonData.update(newData); // Merge objects
		}
		else {
			std::cerr << "[AbilitiesManager] ERROR: JSON type mismatch when merging " << fileName << '\n';
		}
	}

	Ability ReadAbilityInfo(nlohmann::json const& data) {
		// Read information about the ability
		std::string name = data["Name"];
		std::string description = data["Description"];
		AbilityType type = static_cast<AbilityType>(data["Type"].get<int>());
		Tier tier = static_cast<Tier>(data["Tier"].get<int>());

		// Load texture, if no texturename is given use a default texture
		std::string textureName = data["Texture"];
		std::string textureFilePath = "Assets/Abilities/" + textureName + ".png";
		AEGfxTexture* texture = AssetsManager::squareTexture; // default texture
		if (textureName != "") {
			texture = AssetsManager::GetTexture(textureFilePath.c_str());
		}
		
		// ability id suppose to be changed after this function is called
		return Ability{ 0, name, description, type, tier, texture };
	};

	Ability& ReadAllStats(Ability& ability, nlohmann::json const& data) {
		// Set all the stats here
		ability.SetStats(AbilitiesFileManager::ReadBaseStats(data));
		ability.SetProjectile(AbilitiesFileManager::ReadProjectileStats(data));
		ability.SetStatus(AbilitiesFileManager::ReadStatusStats(data));

		return ability;
	}

	StatsModule ReadBaseStats(nlohmann::json const& data) {
		if (data.contains(statsKey) && !data[statsKey].is_null()) {
			auto& s = data[statsKey];
			return StatsModule{
				s["Hp"],
				s["Attack"],
				s["Shield"]
			};
		}

		return StatsModule{}; // return an empty
	}

	ProjectileModule ReadProjectileStats(nlohmann::json const& data) {
		if (data.contains(projectileKey) && !data[projectileKey].is_null()) {
			auto& p = data[projectileKey];
			return ProjectileModule{
				p["ProjectileID"],
				p["Damage_Amp"],
				p["Base_Damage"],
				p["Radius"],
				p["Charges"],
				p["unlimited_charges"]
			};
		}

		return ProjectileModule{};
	}

	StatusModule ReadStatusStats(nlohmann::json const& data) {
		if (data.contains(statusKey) && !data[statusKey].is_null()) {
			auto& status = data[statusKey];
			return StatusModule{
				status["Is_Modifier"],
				status["Rounds"],
				status["Is_Postive"],
				status["Target_Self"],
				status["Gravity_Scale"],
				status["Damage_Each_Round"],
				status["Effect_Radius"],
			};
		}

		return StatusModule{};
	}
}