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

	// Load all abilities from the JSON file and categorize them into their respective vectors
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
			std::string texture = item["Texture"];

			Ability newAbil(id, name, type, tier, texture);
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

		//PrintAllAbilities();
	}

	void AbilityController::PrintAllAbilities() {
		for (Ability ability : statsAbilities) {
			std::cout << "[AbilitiesManager] Loaded stats ability " << ability.Id() << " : " << ability.GetName() << std::endl;
		}
		std::cout << "[AbilitiesManager] Successfully loaded " << statsAbilities.size() << " stats abilities." << std::endl;

		for (Ability ability : offensiveAbilities) {
			std::cout << "[AbilitiesManager] Loaded offensive ability " << ability.Id() << " : " << ability.GetName() << std::endl;
		}
		std::cout << "[AbilitiesManager] Successfully loaded " << offensiveAbilities.size() << " offensive abilities." << std::endl;

		for (Ability ability : defensiveAbilities) {
			std::cout << "[AbilitiesManager] Loaded defensive ability " << ability.Id() << " : " << ability.GetName() << std::endl;
		}
		std::cout << "[AbilitiesManager] Successfully loaded " << defensiveAbilities.size() << " defensive abilities." << std::endl;
	}

	// Load the starting abilities for the player and AI
	void AbilityController::LoadStartingAbilities() {
		if (!isAIController) { // Load player starting Abilities
			AddAbility(AbilityRandomisator(offensiveAbilities).Id());
			AddAbility(AbilityRandomisator(offensiveAbilities).Id());
			AddAbility(AbilityRandomisator(defensiveAbilities).Id());
			AddAbility(AbilityRandomisator(defensiveAbilities).Id());
		}

		std::cout << "[AbilitiesManager] List of starting abilities:" << '\n';
		for (Ability* ability : inventory) {
			std::cout << ability->GetName() << " Charges: " << ability->AbilityCharges() << '\n';
		}
	}

	// Change the current projectile based on the projectile ID
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

	// Use the currently selected projectile if it has unlimited or any charges remaining
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

	// Use the currently selected projectile for the AI if it has unlimited or any charges remaining
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

	// AI Member functions
	void AbilityController::ResetAIAbilitySelection() { hasEnemySelectedAbility = false; }

	// Randomly select an ability for the AI if it has not already selected one
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

	// Change the current ability based on the ability ID
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

	// Use the currently select ability if it has unlimited or any charges remaining
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

	// Add ability charges from the based on selection
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

	// Helper function to add or remove abilities from the inventory vector based on the ability charges
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

	// Return the type of ability that is lesser in inventory, 
	// if 0 or equal then randomly return either offensive or defensive, 
	// if one of them is lesser than the other, return that type
	AbilityType AbilityController::OffensiveOrDefensive() {
		int offensiveCount = 0;
		int defensiveCount = 0;

		for (Ability* ability : inventory) {
			if (ability->GetType() == AbilityType::OFFENSIVE) {
				offensiveCount++;
			}
			else if (ability->GetType() == AbilityType::DEFENSIVE) {
				defensiveCount++;
			}
		}

		if (offensiveCount == defensiveCount) {
			return Utils::RandomValue(0, 1) == 0 ? AbilityType::OFFENSIVE : AbilityType::DEFENSIVE;
		}
		else if (offensiveCount < defensiveCount) {
			return AbilityType::OFFENSIVE;
		}
		else {
			return AbilityType::DEFENSIVE;
		}
	}

	// Helper function to find and return the ability pointer based on the ability ID
	Ability* AbilityController::FindAbility(int abilityId) {
		for (Ability& ab : allAbilities) {
			if (ab.Id() == abilityId) {
				return &ab;
			}
		}

		std::cerr << "[AbilitiesManager] Can't find " + std::to_string(abilityId) + " in the abilities list\n";
		return nullptr;
	}

	// return the projectile Id to change the type of projectile the CANNON is currently using
	int AbilityController::FindProjectileId() {
		if (!currentAbility) return 0;
		// if current ability is a offensive one return the projectile id 
		// otherwise just return the default projectile id which is 0
		if (currentAbility->GetType() != AbilityType::OFFENSIVE) return 0;
		return currentAbility->GetProj().projectileId;
	}

	// True to toggle through all abilities or false to toggle through of all the active abilities this controller has in its inventory
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

	const Ability AbilityController::AbilityRandomisator(const std::vector<Ability> &abilities) {
		return abilities[Utils::RandomValue(0, static_cast<int>(abilities.size() - 1))];
	}

	// Randomly select abilities for the ability selector UI
	void AbilityController::AbilitySelectionRandomisator() {

		std::cout << "[AbilitiesManager] Randomising abilities for the selector..." << std::endl;

		abilitiesCards.clear();

		// Randomise Abilties
		// Check inventory how many offensive and defensive abilities the player has and randomise based on that
		std::cout << "[AbilitiesManager] Number of abilities in inventory: " << inventory.size() << std::endl;

		if (inventory.size() == 5) {
			std::cerr << "[AbilitiesManager] Enough abilities in inventory, randomise stats" << std::endl;
			
			// Currently only 3 stat cards so no need randomise, just push all 3 stat cards into the selector
			for (int i = 0; i < 3; ++i) {
				//abilitiesCards.push_back(AbilityRandomisator(statsAbilities));
				abilitiesCards.push_back(statsAbilities[i]);
			}
		}
		else {
			std::cerr << "[AbilitiesManager] Not enough abilities in inventory, randomise offensive and defensive" << std::endl;
			if (OffensiveOrDefensive() == AbilityType::OFFENSIVE) {
				// 2 offensive and 1 defensive
				abilitiesCards.push_back(AbilityRandomisator(offensiveAbilities));
				abilitiesCards.push_back(AbilityRandomisator(offensiveAbilities));
				abilitiesCards.push_back(AbilityRandomisator(defensiveAbilities));
			}
			else {
				// 2 defensive and 1 offensive
				abilitiesCards.push_back(AbilityRandomisator(defensiveAbilities));
				abilitiesCards.push_back(AbilityRandomisator(defensiveAbilities));
				abilitiesCards.push_back(AbilityRandomisator(offensiveAbilities));
			}
		}



		if (abilitiesCards.size() != 3) {
			std::cerr << "[AbilitiesManager] Incorrect abilities to display in selector!" << std::endl;

			// Fallback to default abilities if there are not enough abilities to randomise
			abilitiesCards.clear();
			abilitiesCards.push_back(offensiveAbilities[0]);
			abilitiesCards.push_back(offensiveAbilities[1]);
			abilitiesCards.push_back(defensiveAbilities[0]);
		}

		// Display the randomised abilities in the console for testing purposes
		int i = 1;
		for (Ability ability : abilitiesCards) {
			std::cout << "[AbilitiesManager] Ability Card " << i << " : "
				<< ability.GetName()
				<< " Type: "; 
			switch (ability.GetType()) {
			case AbilityType::STATISTICS:
				std::cout << "Statistics";
				break;
			case AbilityType::OFFENSIVE:
				std::cout << "Offensive";
				break;
			case AbilityType::DEFENSIVE:
				std::cout << "Defensive";
				break;
			}
			std::cout << std::endl;
			i++;
		}

	}

	// Display the ability selector UI for the player to select an ability
	void AbilityController::AbilitySelector(bool& isOn) {
		if (!isOn) return;

		//std::cout << "Player Selecting Abilities:\n";
		CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

		UIManager::Panel background{ {1400.0f, 700.0f}, Color(0.6196f, 0.6196f, 0.6196f, 1.0f), Utils::AnchorMode::CENTER };
		background.Draw();

		AEVec2 cardSize{ CARD_WIDTH, CARD_HEIGHT }; // width and height
		float gap = (1400.0f - (cardSize.x * abilitiesCards.size())) / (abilitiesCards.size() + 1);
		AEVec2 cardPos{ -cardSize.x * 1.5f - gap, cardSize.y / 2.0f };

		int id = 1;

		for (Ability ability : abilitiesCards) {
			//std::cout << ability.GetName() << '\n';
			AEGfxTexture* imageTexture = ability.GetTexture() == "" ? AssetsManager::squareTexture : AssetsManager::GetTexture(ability.GetTexture().c_str());
			Card card{ ability.GetName(), cardPos, Color(1,0,0), AssetsManager::squareTexture, imageTexture, [this, ability, &isOn]() { onClickAbilityCard(ability, &isOn);  }};
			card.Update();
			card.Draw();
			cardPos.x += cardSize.x + gap;
			id++;
		}
	}

	void AbilityController::onClickAbilityCard(Ability ability, bool* isOn) {
		std::cout << "Clicked ability card " << ability.Id() << std::endl;
		if (ability.GetType() != AbilityType::STATISTICS) {
			AddAbility(ability.Id());
		}
		else {
			// Update stats

		}
		if (isOn) {
			*isOn = false;
		}
		CameraManager::camera.SetState(CameraManager::CameraController::INPUT);
	}

	void AbilityController::abilityUI() {
		UIManager::Panel background{ {600.0f, 90.0f}, Color(0.647f, 0.165f, 0.165f, 1.0f), Utils::AnchorMode::BOTTOM_CENTER };
		background.Draw();

		// Ability icons
		for (int i = 0; i < inventory.size(); ++i) {
			Ability* ability = inventory[i];
			Color iconColor = ability->GetType() == AbilityType::OFFENSIVE ? Color(1, 0, 0) : Color(0, 0, 1);
			std::vector<std::string> toBeRemoved = { "Projectile" };
			std::string AbilityName = ability->GetName();
			for (const std::string& word : toBeRemoved) {
				size_t found = AbilityName.find(word);
				if (found != std::string::npos) {
					AbilityName.erase(found, std::string(word).length());
				}
			};

			// Trim any trailing/leading spaces left behind
			while (!AbilityName.empty() && AbilityName.back() == ' ') AbilityName.pop_back();
			while (!AbilityName.empty() && AbilityName.front() == ' ') AbilityName.erase(AbilityName.begin());

			Button abilityButton{ TextObject{ AbilityName },
				Utils::AnchorMode::BOTTOM_CENTER, AEVec2{ -220.0f + i * 100.0f, -370.0f },
				AEVec2{ 75.0f, 75.0f }, iconColor, AssetsManager::squareTexture };
			abilityButton.SetTextScale(0.5f);
			abilityButton.Update();
			abilityButton.Draw();
		}
		
		int i = static_cast<int>(inventory.size());
		while (i < 5) {
			Button emptyButton{ TextObject{ "Empty" },
				Utils::AnchorMode::BOTTOM_CENTER, AEVec2{ -220.0f + i * 100.0f, -370.0f },
				AEVec2{ 75.0f, 75.0f }, Color(0.5f,0.5f,0.5f), AssetsManager::squareTexture };
			emptyButton.SetTextScale(0.5f);
			emptyButton.Update();
			emptyButton.Draw();
			i++;
		}
	}
}

