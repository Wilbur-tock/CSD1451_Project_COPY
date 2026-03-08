#include "AbilitiesManager.hpp"

#include "json.hpp"

#include "Utils.hpp"
#include "Color.hpp"
#include "UIManager.hpp"
#include "Card.hpp"
#include "Button.hpp"
#include "AssetsManager.hpp"
#include "CameraManager.hpp"

namespace {
	// Helper function to read from JSON file and input the data into a JSON Object
	static void ReadAbilitiesFromFile(nlohmann::json& jsonData, std::string const& filePath) {
		// Open the file
		std::ifstream file(filePath);
		if (!file.is_open()) {
			std::cerr << "[AbilitiesManager] ERROR: Could not open " << filePath << '\n';
			return;
		}

		// Parse the file into a JSON object
		//file >> jsonData;

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
			std::cerr << "[AbilitiesManager] ERROR: JSON type mismatch when merging " << filePath << '\n';
		}
	}
}

namespace AbilitiesManager {
	AbilityController playerAbilityController;
	AbilityController enemyAbilityController;

	void AbilityController::LoadAllAbilities(bool isAi) {
		offensiveAbilities.clear();
		statsAbilities.clear();
		defensiveAbilities.clear();

		allAbilities.clear();
		allAbilities.reserve(64);
		inventory.clear();
		currentAbility = nullptr;

		isAIController = isAi;

		nlohmann::json data;
		ReadAbilitiesFromFile(data, "Assets/Abils/Abilities.json");

		// Read enemies ONLY ability and merge it to the data
		if (isAi) {
			ReadAbilitiesFromFile(data, "Assets/Abils/AiAbilities.json");
		}

		int id = 0;
		for (auto& item : data) { // Your JSON is an array at the root
			std::string name = item["Name"];
			AbilityType type = static_cast<AbilityType>(item["Type"].get<int>());
			Tier tier = static_cast<Tier>(item["Tier"].get<int>());

			Ability newAbil(id,name, type, tier);
			++id;

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

			allAbilities.push_back(newAbil);
		}
		std::cout << "[AbilitiesManager] Successfully loaded " << statsAbilities.size() << " stats abilities." << std::endl;
		std::cout << "[AbilitiesManager] Successfully loaded " << offensiveAbilities.size() << " offensive abilities." << std::endl;
		std::cout << "[AbilitiesManager] Successfully loaded " << defensiveAbilities.size() << " defensive abilities." << std::endl;

		LoadStartingAbilities();
	}

	void AbilityController::LoadStartingAbilities() {
		if (!isAIController) { // Load player starting Abilities
			AddAbility(0);
			AddAbility(1);
			AddAbility(2);
		}

		std::cout << "[AbilitiesManager] List of starting abilities:" << '\n';
		for (Ability* ability : inventory) {
			std::cout << ability->GetName() << " Charges: " << ability->AbilityCharges() << '\n';
		}
	}

	/*
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
	*/

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

	void AbilityController::ResetAIAbilitySelection() { hasEnemySelectedAbility = false; }

	void AbilityController::AIAbilitySelection() {
		if (hasEnemySelectedAbility) return;

		int selectedId = 0;
		if (allAbilities.empty()) {
			std::cerr << "[AbilitiesManager] No abilities loaded!" << '\n';
			return;
		}
		// Randomly select abilities for now
		int size = static_cast<int>(allAbilities.size());;
		if (size > 1) {
			selectedId = Utils::RandomValue(0, size - 1);
		}
		ChangeAbility(selectedId);
		hasEnemySelectedAbility = true;

		std::cout << "[AbilitiesManager] Enemy selected ability: " << currentAbility->GetName() << '\n';
	}

	void AbilityController::ChangeAbility(int abilityId) {
		Ability* found = FindAbility(abilityId);
		if (!found) {
			std::cerr << "[AbilitiesManager] ChangeAbility failed. abilityId=" << abilityId << '\n';
			return;
		}
		currentAbility = found;

		std::cout << "[AbilitiesManager] Current ability: " + currentAbility->GetName()
			+ " Charges Remaining: " + std::to_string(currentAbility->AbilityCharges())
 			<< '\n';
	}

	bool AbilityController::UseAbility() {
		if (!currentAbility) {
			std::cerr << "[AbilitiesManager] Use Ability failed. currentAbility is nullptr\n";
			return false;
		}
		if (currentAbility->GetProj().unlimited_charges) return true;
		if (currentAbility->AbilityCharges() <= 0) return false;

		currentAbility->AbilityCharges(-1);
		UpdateInventory(*currentAbility, false);

		if (currentAbility->GetType() == AbilityType::OFFENSIVE && currentAbility->AbilityCharges() <= 0) {
			ChangeAbility(0);
		}

		std::cout << "[AbilitiesManager] Ability used: " + currentAbility->GetName()
			+ " Charges Remaining: " + std::to_string(currentAbility->AbilityCharges()) << '\n';
		return true;
	}

	void AbilityController::AddAbility(int abilityId, int chargesToAdd) {
		Ability* found = FindAbility(abilityId);
		if (!found) {
			std::cerr << "[AbilitiesManager] Add Ability failed. abilityId=" << abilityId << '\n';
			return;
		}
		Ability& ability = *found;
		ability.AbilityCharges(chargesToAdd);

		UpdateInventory(ability, true);
	}

	void AbilityController::UpdateInventory(Ability& ability, bool toAddToVector) {
		if (toAddToVector) {
			// if previous charges is already more than or equal to 1, means that it is already in the vector no need to add
			if (ability.PreviousAbilityCharges() >= 1) return;

			// add into the activeAbilities vector
			inventory.push_back(&ability);

			std::cout << "[AbilitiesManager] Added " << ability << " into active abilities vector" << '\n';
		}
		else {
			if (ability.AbilityCharges() > 0) return;
			for (int i = 0; i < inventory.size(); ++i) {
				if (inventory[i] == &ability) {
					inventory.erase(inventory.begin() + i);
					std::cout << "[AbilitiesManager] Removed " << ability << " from active abilities vector" << '\n';
					return;
				}
			}
		}
	}

	Ability* AbilityController::FindAbility(int abilityId) {
		for (Ability& ab : allAbilities) {
			if (ab.Id() == abilityId) {
				return &ab;
			}
		}

		std::cerr << "[AbilitiesManager] Can't find " + std::to_string(abilityId) + " in the abilities list\n";
		return nullptr;
	}

	int AbilityController::FindProjectileId() {
		if (!currentAbility) return 0;
		// if current ability is a offensive one return the projectile id 
		// otherwise just return the default projectile id which is 0
		if (currentAbility->GetType() != AbilityType::OFFENSIVE) return 0;
		return currentAbility->GetProj().projectileId;
	}

	void AbilityController::ToggleAbility(bool isAllAbilities) {
		int abilityId = currentAbility->Id();

		if (isAllAbilities) {
			Utils::Wrap(++abilityId, 0, static_cast<int>(allAbilities.size()-1));
		}
		else {
			for (int i = 0; i < inventory.size(); ++i) {
				if (inventory[i] == currentAbility) {
					int selectedListId = i + 1;
					Utils::Wrap(selectedListId, 0, static_cast<int>(inventory.size() - 1));
					abilityId = inventory[selectedListId]->Id();
					break;
				}
			}
		}
		ChangeAbility(abilityId);
	}

	void AbilityController::AbilitySelector(bool& isOn) {
		if (!isOn) return;

		//std::cout << "Player Selecting Abilities:\n";
		CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

		// Randomise Abilties
		std::vector<std::string> abilitiesName = { "Default Projectile", "Bomb Projectile", "Fast Projectile" };

		// 2 offensive abilities for the player to choose from
		// 1 defensive ability for the player to choose from

		// randomise the attack abilities
		
		std::vector<Ability> abilitiesVec = {};

		abilitiesVec.push_back(offensiveAbilities[0]);
		abilitiesVec.push_back(offensiveAbilities[1]);
		abilitiesVec.push_back(defensiveAbilities[0]);
		
		if (abilitiesVec.size() != 3) {
			std::cerr << "[AbilitiesManager] Incorrect abilities to display in selector!" << std::endl;
		}

		UIManager::Panel background{ {1400.0f, 700.0f}, Color(0.6196f, 0.6196f, 0.6196f, 1.0f), Utils::AnchorMode::CENTER };
		background.Draw();

		AEVec2 cardSize{ 350.0f, 550.0f }; // width and height
		float gap = (1400.0f - (cardSize.x * abilitiesName.size())) / (abilitiesName.size() + 1);
		AEVec2 cardPos{ -cardSize.x * 1.5f - gap, cardSize.y / 2.0f };

		int id = 1;

		for (Ability ability : abilitiesVec) {
			//std::cout << abilityName << '\n';
			Card card{ ability.GetName(), cardPos, Color(1,0,0), AssetsManager::squareTexture, AssetsManager::squareTexture, [this, id, &isOn]() { onClickAbilityCard(id, &isOn);  }};
			card.Update();
			card.Draw();
			cardPos.x += cardSize.x + gap;
			id++;
		}
	}

	void AbilityController::onClickAbilityCard(int abilityID, bool* isOn) {
		std::cout << "Clicked ability card" << abilityID << std::endl;
		if (isOn) {
			*isOn = false;
		}
		CameraManager::camera.SetState(CameraManager::CameraController::INPUT);
	}
}








	/*
	void AbilityController::LoadAllAbilities() {
		abilities.clear();
		activeAbilities.clear();
		currentAbility = nullptr;
		hasEnemySelectedAbility = false;

		int count = 3;
		abilities.reserve(count);
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
		Ability* defaultAbility = FindAbility(0);
		if (defaultAbility) {
			currentAbility = defaultAbility;
		}
		else {
			std::cerr << "[AbilitiesManager] Failed to set default ability!\n";
		}
	}

	void AbilityController::ChangeAbility(int abilityId) {
		Ability* found = FindAbility(abilityId);
		if (!found) {
			std::cerr << "[AbilitiesManager] ChangeAbility failed. abilityId=" << abilityId << '\n';
			return;
		}
		currentAbility = found;

		std::cout << "[AbilitiesManager] Current ability: " + abilitiesName[currentAbility->Id()]
			+ " Charges Remaining: " + std::to_string(currentAbility->Charges()) << '\n';
	}

	bool AbilityController::UseAbility() {
		if (!currentAbility) {
			std::cerr << "[AbilitiesManager] UseAbility failed. currentAbility is nullptr\n";
			return false;
		}
		if (currentAbility->UnlimitedCharges()) return true;
		if (currentAbility->Charges() <= 0) return false;

		currentAbility->Charges(-1);
		UpdateActiveAbilites(*currentAbility, false);

		if (currentAbility->Type() == AbilityType::OFFENSIVE && currentAbility->Charges() <= 0) {
			ChangeAbility(0);
		}

		std::cout << "[AbilitiesManager] Ability used: " + abilitiesName[currentAbility->Id()]
			+ " Charges Remaining: " + std::to_string(currentAbility->Charges()) << '\n';
		return true;
	}

	void AbilityController::AddAbility(int abilityId, int chargesToAdd) {
		Ability* found = FindAbility(abilityId);
		if (!found) {
			std::cerr << "[AbilitiesManager] AddAbility failed. abilityId=" << abilityId << '\n';
			return;
		}
		Ability& ability = *found;
		ability.Charges(chargesToAdd);

		UpdateActiveAbilites(ability, true);
	}

	int AbilityController::FindProjectileId() {
		if (!currentAbility) return 0;
		if (currentAbility->Type() != AbilityType::OFFENSIVE) return 0;
		return currentAbility->ProjectileId();
	}

	void AbilityController::UpdateActiveAbilites(Ability& ability, bool toAddToVector) {
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

		int size = static_cast<int>(activeAbilities.size());
		if (size <= 0) {
			std::cerr << "[AbilitiesManager] Enemy has no active abilities to choose from!\n";
			currentAbility = FindAbility(0); // fallback
			hasEnemySelectedAbility = true;
			return;
		}

		int selectedId = 0;
		if (size > 1) {
			// If RandomValue can ever return "size", clamp prevents out-of-range crash
			selectedId = Utils::RandomValue(0, size);
			selectedId = std::clamp(selectedId, 0, size - 1);
		}

		currentAbility = activeAbilities[selectedId];
		hasEnemySelectedAbility = true;

		std::cout << "[AbilitiesManager] Enemy selected ability: " << *currentAbility << '\n';
	}

	
}


*/