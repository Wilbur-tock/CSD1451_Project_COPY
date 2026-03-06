#include "AbilitiesManager.hpp"

#include "Utils.hpp"
#include "Color.hpp"
#include "UIManager.hpp"

namespace AbilitiesManager {
	AbilityController playerAbilityController;
	AbilityController enemyAbilityController;


	void AbilityController::LoadAllAbilities() {
		offensiveAbilities.clear();
		statsAbilities.clear();
		defensiveAbilities.clear();

		// 2. Open the file
		std::ifstream file("Assets/Abils/Abilities.json");
		if (!file.is_open()) {
			std::cerr << "[AbilitiesManager] ERROR: Could not open Abilities.json" << std::endl;
			return;
		}

		// 3. Parse the file into a JSON object
		nlohmann::json data;
		file >> data;

		
		for (auto& item : data) { // Your JSON is an array at the root
			std::string name = item["Name"];
			AbilityType type = static_cast<AbilityType>(item["Type"].get<int>());
			Tier tier = static_cast<Tier>(item["Tier"].get<int>());

			Ability newAbil(name, type, tier);

			// ROUTE based on Type
			if (type == AbilityType::STATISTICS) {
				auto& s = item["Base_Stats"];
				newAbil.SetStats(s["HP"], s["ATK"], s["DEF"]);
				statsAbilities.push_back(newAbil);
			}
			else if (type == AbilityType::OFFENSIVE) {
				auto& p = item["Projectile_Stats"];
				newAbil.SetProjectile(
					p["ProjectileID"],
					p["Damage_Amp"],
					p["Base_Damage"],
					p["Radius"],
					p["Charges"],
					p["unlimited_charges"]
				);
				offensiveAbilities.push_back(newAbil);
			}
			else if (type == AbilityType::DEFENSIVE) {
				auto& d = item["Defensive_Stats"];
				newAbil.SetDefense(d["ShieldHP"]);
				defensiveAbilities.push_back(newAbil);
			}

		}
		std::cout << "[AbilitiesManager] Successfully loaded " << statsAbilities.size() << " player stats abilities." << std::endl;
		std::cout << "[AbilitiesManager] Successfully loaded " << offensiveAbilities.size() << " player offensive abilities." << std::endl;
		std::cout << "[AbilitiesManager] Successfully loaded " << defensiveAbilities.size() << " player defensive abilities." << std::endl;
	}

	void AbilityController::LoadAllAIAbilities() {
		AIoffensiveAbilities.clear();

		// 2. Open the file
		std::ifstream file("Assets/Abils/AIAbilities.json");
		if (!file.is_open()) {
			std::cerr << "[AbilitiesManager] ERROR: Could not open AIAbilities.json" << std::endl;
			return;
		}

		// 3. Parse the file into a JSON object
		nlohmann::json data;
		file >> data;

		for (auto& item : data) { // Your JSON is an array at the root
			std::string name = item["Name"];
			AbilityType type = static_cast<AbilityType>(item["Type"].get<int>());
			Tier tier = static_cast<Tier>(item["Tier"].get<int>());

			Ability newAbil(name, type, tier);

			// ROUTE based on Type
			if (type == AbilityType::STATISTICS) {
				auto& s = item["Base_Stats"];
				newAbil.SetStats(s["HP"], s["ATK"], s["DEF"]);
				AIstatsAbilities.push_back(newAbil);
			}
			else if (type == AbilityType::OFFENSIVE) {
				auto& p = item["Projectile_Stats"];
				newAbil.SetProjectile(
					p["ProjectileID"],
					p["Damage_Amp"],
					p["Base_Damage"],
					p["Radius"],
					p["Charges"],
					p["unlimited_charges"]
				);
				AIoffensiveAbilities.push_back(newAbil);
			}
			else if (type == AbilityType::DEFENSIVE) {
				auto& d = item["Defensive_Stats"];
				newAbil.SetDefense(d["ShieldHP"]);
				AIdefensiveAbilities.push_back(newAbil);
			}

		}
		std::cout << "[AbilitiesManager] Successfully loaded " << AIstatsAbilities.size() << " AI stats abilities." << std::endl;
		std::cout << "[AbilitiesManager] Successfully loaded " << AIoffensiveAbilities.size() << " AI offensive abilities." << std::endl;
		std::cout << "[AbilitiesManager] Successfully loaded " << AIdefensiveAbilities.size() << " AI defensive abilities." << std::endl;
	}

	void AbilityController::ChangeProjectile(s32 _projectileID) {
		for (size_t i = 0; i < offensiveAbilities.size(); ++i) {
			if (offensiveAbilities[i].GetProj().projectileId == _projectileID) {
				ProjectileSelected = &offensiveAbilities[i];
				std::cout << "[AbilitiesManager] Switched to: " << ProjectileSelected->GetName() << std::endl;
				return;
			}
		}
		std::cerr << "[AbilitiesManager] ERROR: Projectile ID " << _projectileID << " not found in offensive list!" << std::endl;
	}

	bool AbilityController::UseProjectile() {
		if (ProjectileSelected == nullptr) return false;

		auto projData = ProjectileSelected->GetProj();

		if (!projData.unlimited_charges) {
			if (projData.charges <= 0) {
				std::cout << "Out of charges for " << ProjectileSelected->GetName() << "!" << std::endl;
				return false;
			}

			ProjectileSelected->UpdateCharges(-1);
		}

		std::cout << "Used " << ProjectileSelected->GetName() << ". Charges left: " << ProjectileSelected->GetProj().charges << std::endl;

		return true;
	}

	bool AbilityController::AIUseProjectile() {
		if (AIProjectileSelected == nullptr) return false;

		auto projData = AIProjectileSelected->GetProj();

		if (!projData.unlimited_charges) {
			if (projData.charges <= 0) {
				std::cout << "Out of charges for " << AIProjectileSelected->GetName() << "!" << std::endl;
				return false;
			}

			AIProjectileSelected->UpdateCharges(-1);
		}

		std::cout << "Used " << AIProjectileSelected->GetName() << ". Charges left: " << AIProjectileSelected->GetProj().charges << std::endl;

		return true;
	}

	void AbilityController::ResetAbilitySelection() { hasEnemySelectedAbility = false; }

	void AbilityController::AIAbilitySelection() {
		if (hasEnemySelectedAbility) return;

		int selectedId = 0;
		if (AIoffensiveAbilities.empty()) {
			std::cerr << "[AbilitiesManager] No AI abilities loaded!" << '\n';
			return;
		}
		int size = static_cast<int>(AIoffensiveAbilities.size());;
		if (size > 1) {
			selectedId = Utils::RandomValue(0, size - 1);
		}
		AIProjectileSelected = &AIoffensiveAbilities[selectedId];
		hasEnemySelectedAbility = true;

		std::cout << "[AbilitiesManager] Enemy selected ability: " << AIProjectileSelected->GetName() << '\n';
	}










	/*
	void AbilityController::LoadAllAbilities() {
		int count = 3;
		for (int i = 0; i < count; ++i) {
			int defaultProjectileId = 0; // DEFAULT PROJECTILE id should always be 0
			Ability ability{ i, abilitiesName[i], AbilityType::OFFENSIVE, defaultProjectileId, true };

			switch (i) {
			case 0: // Default Projectile
				break;
			case 1: // Bomb Projectile
				ability = { i, abilitiesName[i], AbilityType::OFFENSIVE, 1 };
				break;
			case 2:
				ability = { i, abilitiesName[i], AbilityType::OFFENSIVE, 2 };
				break;
			}

			abilities.push_back(ability);
		}

		AddAbility(0, 1); // Unlimited default projectile ability
		// Testing add bomb projectile with 10 charges
		AddAbility(1, 10);
		AddAbility(2, 10);
		// Set current ability to be the default projectile ability
		currentAbility = &abilities[0];
	}

	void AbilityController::ChangeAbility(int abilityId) {
		currentAbility = FindAbility(abilityId);

		std::cout << "[AbilitiesManager] Current ability: " + abilitiesName[currentAbility->Id()]
			+ " Charges Remaining: " + std::to_string(currentAbility->Charges()) << '\n';
	}

	bool AbilityController::UseAbility() {
		if (currentAbility->UnlimitedCharges()) return true;
		if (currentAbility->Charges() <= 0) return false;

		currentAbility->Charges(-1);
		UpdateActiveAbilites(*currentAbility, false);

		// Update projectile to DEFAULT projectile once this ability charges is all used up
		if (currentAbility->Type() == AbilityType::OFFENSIVE && currentAbility->Charges() <= 0) {
			ChangeAbility(0);
		}

		std::cout << "[AbilitiesManager] Ability used: " + abilitiesName[currentAbility->Id()] 
			+ " Charges Remaining: " + std::to_string(currentAbility->Charges()) << '\n';

		return true;
	}

	void AbilityController::AddAbility(int abilityId, int chargesToAdd) {
		Ability& ability = *FindAbility(abilityId);
		ability.Charges(chargesToAdd);

		UpdateActiveAbilites(ability, true);
	}

	void AbilityController::UpdateActiveAbilites(Ability &ability, bool toAddToVector) {
		if (toAddToVector) {
			// if previous charges is already more than or equal to 1, means that it is already in the vector no need to add
			if (ability.PreviousCharges() >= 1) return; 

			// add into the activeAbilities vector
			activeAbilities.push_back(&ability);

			std::cout << "[AbilitiesManager] Added " << ability << " into active abilities vector" << '\n';
		}
		else {
			if (ability.Charges() > 0) return; 
			for (int i = 0; i < activeAbilities.size(); ++i) {
				if (activeAbilities[i] == &ability) {
					activeAbilities.erase(activeAbilities.begin() + i);
					std::cout << "[AbilitiesManager] Removed " << ability << " from active abilities vector" << '\n';
					return;
				}
			}
		}
	}

	Ability* AbilityController::FindAbility(int abilityId) {
		for (Ability& ab : abilities) {
			if (ab.Id() == abilityId) {
				return &ab;
			}
		}

		std::cerr << "[AbilitiesManager] Can't find " + std::to_string(abilityId) + " in the abilities list\n";
		return nullptr;
	}

	int AbilityController::FindProjectileId() {
		if (currentAbility->Type() != AbilityType::OFFENSIVE) return 0;
		return currentAbility->ProjectileId();
	}

	void SetProjectileAbility(Projectile& projectile) {
		switch (projectile.Mode()) {
		case ProjectileMode::ATTACK:
			projectile.attribute.Damage(10);
			projectile.attribute.DamageRadius(1);
			break;
		case ProjectileMode::BOMB: // 3 tile radius AOE Instant damage
			projectile.attribute.DamageRadius(3);
			projectile.attribute.Damage(10);
			break;
		case ProjectileMode::FAST:
			projectile.attribute.DamageRadius(5);
			projectile.attribute.Damage(50);
			break;
		}
	}

	void AbilityController::ResetAbilitySelection() { hasEnemySelectedAbility = false; }

	void AbilityController::AIAbilitySelection() {
		if (hasEnemySelectedAbility) return;

		int selectedId = 0;
		int size = static_cast<int>(activeAbilities.size());;
		if (size > 1) {
			selectedId = Utils::RandomValue(0, size);
		}
		currentAbility = activeAbilities[selectedId];
		hasEnemySelectedAbility = true;

		std::cout << "[AbilitiesManager] Enemy selected ability: " << *currentAbility << '\n';
	}
	*/
}