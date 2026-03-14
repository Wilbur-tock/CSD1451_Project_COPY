#include "AbilitiesManager.hpp"

#include "json.hpp"

#include "Utils.hpp"
#include "Color.hpp"
#include "UIManager.hpp"
#include "Card.hpp"
#include "AssetsManager.hpp"
#include "CameraManager.hpp"
#include "CollisionManager.hpp"
#include "LevelManager.hpp"

namespace {
	std::string filePath = "Assets/Abilities/";
	// Helper function to read from JSON file and input the data into a JSON Object
	static void ReadAbilitiesFromFile(nlohmann::json& jsonData, std::string const& fileName) {
		std::string filePathAndName = filePath + fileName;
		// Open the file
		std::ifstream file(filePathAndName);
		if (!file.is_open()) {
			std::cerr << "[AbilitiesManager] ERROR: Could not open " << filePathAndName << '\n';
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
			std::cerr << "[AbilitiesManager] ERROR: JSON type mismatch when merging " << fileName << '\n';
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
		inventory.clear();
		currentAbility = nullptr;

		// Clear inventory state before adding starting abilities
		inventory.clear();
		inventoryAbilityIds.clear();
		inventoryCount = 0;
		std::fill(inventoryTypeCount.begin(), inventoryTypeCount.end(), 0);

		inventory.reserve(Config::MAX_INVENTORY_COUNT);
		inventoryAbilityIds.reserve(Config::MAX_INVENTORY_COUNT);

		isAIController = isAi;

		// Reading of data from json file
		nlohmann::json data;
		ReadAbilitiesFromFile(data, "Abilities.json");

		// Read enemies ONLY ability and merge it to the data
		if (isAi) {
			controllerName = "Enemy Ability Controller";
			ReadAbilitiesFromFile(data, "AiAbilities.json");
		}
		else {
			controllerName = "Player Ability Controller";
			ReadAbilitiesFromFile(data, "PlayerAbilities.json");
		}

		totalAbilityCount = data.size();
		// Reserve space for all vector used in these controller
		allProjectilesAbilties.reserve(totalAbilityCount);
		allAbilities.reserve(totalAbilityCount);
		offensiveAbilities.reserve(totalAbilityCount);
		statsAbilities.reserve(totalAbilityCount);
		defensiveAbilities.reserve(totalAbilityCount);

		int id = 0;
		for (auto& item : data) { // Your JSON is an array at the root
			std::string name = item["Name"];
			AbilityType type = static_cast<AbilityType>(item["Type"].get<int>());
			Tier tier = static_cast<Tier>(item["Tier"].get<int>());
			std::string texture = item["Texture"];

			// Increment Id after assigning it
			Ability newAbility(id++, name, type, tier, texture);

			// Load different stats if can find
			std::string statsName = "Base_Stats";
			if (item.contains(statsName) && !item[statsName].is_null()) {
				auto& s = item[statsName];
				newAbility.SetStats(StatsModule{
					s["Hp"], s["Attack"], s["Shield"], s["Turns"]
				});
			}

			std::string projectileName = "Projectile_Stats";
			if (item.contains(projectileName) && !item[projectileName].is_null()) {
				auto& p = item[projectileName];
				newAbility.SetProjectile(ProjectileModule{
					p["ProjectileID"],
					p["Damage_Amp"],
					p["Base_Damage"],
					p["Radius"],
					p["Charges"],
					p["unlimited_charges"]
				});
			}

			std::string defensiveName = "Defensive_Stats";
			if (item.contains(defensiveName) && !item[defensiveName].is_null()) {
				auto& d = item[defensiveName];
				newAbility.SetDefense(DefenseModule{ 
					d["ShieldHP"] 
				});
			}

			// Add to the type of ability list
			switch (type) {
			case AbilityType::STATISTICS:
				statsAbilities.push_back(newAbility);
				break;
			case AbilityType::OFFENSIVE:
				offensiveAbilities.push_back(newAbility);
				break;
			case AbilityType::DEFENSIVE:
				defensiveAbilities.push_back(newAbility);
				break;
			}

			allAbilities.push_back(newAbility);

			// Only add the default projectile ability as non-projectile abiility would use default projectile id
			if (newAbility.GetProj().projectileId == 0 && id != 0) {
				allProjectilesAbilties.push_back(newAbility);
			}

			// Do not adding default projectile again after it has been added
			if (newAbility.GetProj().projectileId != 0) { 
				allProjectilesAbilties.push_back(newAbility);
			}
		}

		inventoryTypeCount.resize(static_cast<int>(AbilityType::COUNT));
		//PrintAllAbilities();

		LoadStartingAbilities();
	}

	// Load the starting abilities for the player and AI
	void AbilityController::LoadStartingAbilities() {
		if (!isAIController) { // Load player starting Abilities
			AddAbility(AbilityRandomisator(offensiveAbilities).Id());
			AddAbility(AbilityRandomisator(offensiveAbilities).Id());
			AddAbility(AbilityRandomisator(defensiveAbilities).Id());
			AddAbility(AbilityRandomisator(defensiveAbilities).Id());
		}

		//std::cout << "[AbilitiesManager] List of starting abilities:" << '\n';
		//for (Ability* ability : inventory) {
		//	std::cout << ability->GetName() << " Charges: " << ability->AbilityCharges() << '\n';
		//}
	}

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
			std::cerr << "[AbilitiesManager] Change Ability failed. abilityId: " << abilityId << '\n';
			return;
		}
		currentAbility = found;

		//std::cout << "[AbilitiesManager] Current ability: " + currentAbility->GetName()
		//	+ " Charges Remaining: " + std::to_string(currentAbility->AbilityCharges()) << '\n';
	}

	// Use the currently select ability if it has unlimited or any charges remaining
	bool AbilityController::UseAbility() {
		if (!currentAbility) {
			std::cerr << "[AbilitiesManager] Use Ability failed. currentAbility is nullptr\n";
			return false;
		}
		if (currentAbility->GetProj().unlimitedCharges) return true;
		if (currentAbility->AbilityCharges() <= 0) return false; // has no ability charges can't use this ability

		currentAbility->AbilityCharges(-1);

		UpdateInventory(*currentAbility, false);

		// Update inventory count of ability type
		int id = static_cast<int>(currentAbility->GetType()) - 1;
		--inventoryTypeCount[id]; 
		--inventoryCount;
		// Reset to default if trying to use an ability without any charges remaining
		ResetAbility();

		std::cout << "[AbilitiesManager] Ability used: " + currentAbility->GetName()
			+ " Charges Remaining: " + std::to_string(currentAbility->AbilityCharges()) << '\n';
		return true;
	}

	// Add ability charges from the based on selection
	void AbilityController::AddAbility(int abilityId, int chargesToAdd) {
		// Don't add any new ability after inventory count has reach the max
		if (IsInventoryFull()) {
			std::cout << "[AbilitesManager] Inventory is full can't add any new ability!" << '\n';
			return; 
		}
		Ability* found = FindAbility(abilityId);
		if (!found) {
			std::cerr << "[AbilitiesManager] Can't find to add Ability Id: " + std::to_string(abilityId) << '\n';
			return;
		}
		// Find ability and add the charges
		Ability& ability = *found;
		ability.AbilityCharges(chargesToAdd);

		int id = static_cast<int>(ability.GetType()) - 1;
		++inventoryTypeCount[id];
		++inventoryCount;

		UpdateInventory(ability, true);
	}

	// Helper function to add or remove abilities from the inventory vector based on the ability charges
	void AbilityController::UpdateInventory(Ability& ability, bool toAddToVector) {
		if (toAddToVector) {
			std::cout << "[AbilitiesManager] Added " << ability << " into inventory" << '\n';
			inventoryAbilityIds.push_back(ability.Id());

			// if previous charges is already more than or equal to 1, means that it is already in the vector no need to add
			if (ability.PreviousAbilityCharges() >= 1) return;

			// add into the activeAbilities vector
			inventory.push_back(&ability);

		}
		else {
			std::cout << "[AbilitiesManager] Removed " << ability << " from inventory" << '\n';
			// Remove from inventoryAbilityIds vector
			for (int i = currentInventoryId; i < Config::MAX_INVENTORY_COUNT; ++i) {
				if (i < Config::MAX_INVENTORY_COUNT - 1) {
					inventoryAbilityIds[i] = inventoryAbilityIds[i + 1]; // shift every currentInventoryId + 1 foward by 1
				}
				else {
					inventoryAbilityIds.pop_back(); // remove the last vector
				}
			}

			// Erase from inventoryAbilties Vector
			if (ability.AbilityCharges() > 0) return; // if there is ability charges remanining no need to erase from inventory vector
			for (int i = 0; i < inventory.size(); ++i) {
				if (inventory[i] == &ability) {
					inventory.erase(inventory.begin() + i);
					return;
				}
			}
		}
	}

	// Return the type of ability that is lesser in inventory, 
	// if 0 or equal then randomly return either offensive or defensive, 
	// if one of them is lesser than the other, return that type
	AbilityType AbilityController::OffensiveOrDefensive() {
		int offensiveCount = inventoryTypeCount[static_cast<int>(AbilityType::OFFENSIVE) - 1];
		int defensiveCount = inventoryTypeCount[static_cast<int>(AbilityType::DEFENSIVE) - 1];

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
	int AbilityController::GetProjectileId() {
		if (!currentAbility) return 0;
		return currentAbility->GetProj().projectileId;
	}

	// True to toggle through all abilities or false to toggle through of all the active abilities this controller has in its inventory
	void AbilityController::ToggleAbility(bool isAllAbilities, int addValue) {
		int abilityId = currentAbility->Id();
		abilityId += addValue;

		if (isAllAbilities) {
			Utils::Wrap(abilityId, 0, static_cast<int>(allAbilities.size()-1));
		}
		else {
			bool found = false;
			for (int i = 0; i < inventory.size(); ++i) {
				if (inventory[i] == currentAbility) {
					int selectedListId = i + addValue;
					Utils::Wrap(selectedListId, 0, static_cast<int>(inventory.size() - 1));
					abilityId = inventory[selectedListId]->Id();
					found = true;
					break;
				}
			}

			if(!found) abilityId = 0; // Reset to default ability if can't find any
		}
		ChangeAbility(abilityId);
	}

	const Ability AbilityController::AbilityRandomisator(std::vector<Ability> const& abilities) {
		return abilities[Utils::RandomValue(0, static_cast<int>(abilities.size() - 1))];
	}

	// Randomly select abilities for the ability selector UI
	void AbilityController::AbilitySelectionRandomisator() {

		std::cout << "[AbilitiesManager] Randomising abilities for the selector..." << std::endl;

		abilitiesCards.clear();

		// Randomise Abilties
		// Check inventory how many offensive and defensive abilities the player has and randomise based on that
		std::cout << "[AbilitiesManager] Number of abilities in inventory: " << inventory.size() << std::endl;

		if (inventory.size() >= Config::MAX_INVENTORY_COUNT) {
			std::cerr << "[AbilitiesManager] Enough abilities in inventory, randomise stats" << std::endl;

			//// Currently only 3 stat cards so no need randomise, just push all 3 stat cards into the selector
			//for (int i = 0; i < 3; ++i) {
			//	//abilitiesCards.push_back(AbilityRandomisator(statsAbilities));
			//	abilitiesCards.push_back(statsAbilities[i]);
			//}
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
	void AbilityController::AbilitySelector() {
		if (!isSelectingAbility) return;

		//std::cout << "Player Selecting Abilities:\n";
		CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

		AbilitySelectorUI();
	}

	void AbilityController::OnClickAbilityCard(Ability const& ability) {
		if (ability.GetType() != AbilityType::STATISTICS) {
			AddAbility(ability.Id());
		}
		else {
			// Update stats

		}

		isSelectingAbility = false;
		CameraManager::camera.SetState(CameraManager::CameraController::INPUT);
	}

	static constexpr int inventoryMaxCount = Config::MAX_INVENTORY_COUNT;
	static constexpr int selectorCount = Config::MAX_ABILITY_SELECTOR_COUNT;
	// UI OBJECTS
	UIManager::Panel inventoryBackground;
	Button inventoryAbilityButton[inventoryMaxCount];

	// Selecting Ability UI
	UIManager::Panel abilitySelectorBG;
	Card abilitySelectorCards[selectorCount];

	void AbilityController::InitializeAllAbilityUI() {
		inventoryBackground = UIManager::Panel{ {600.0f, 100.0f}, Color(0.647f, 0.165f, 0.165f, 1.0f), Utils::AnchorMode::BOTTOM_CENTER };

		Transform inventoryBGTrans = inventoryBackground.GetTransform();
		float totalWidth = inventoryBGTrans.size.x;

		// Values to edit
		float space = 15.0f;
		float borderPaddingX = 25.0f;
		float borderPaddingY = 5.0f;

		// Calculate total width of spaces between buttons
		// [BTN][SPACE][BTN][SPACE].....
		float spaceTotalWidth = space * (inventoryMaxCount - 1);
		float totalBtnWidth = totalWidth - spaceTotalWidth - (borderPaddingX * 2.0f);

		float btnSizeX = totalBtnWidth / inventoryMaxCount;
		float btnSizeY = inventoryBGTrans.size.y - (borderPaddingY * 2.0f);

		// Set starting position
		float startX = inventoryBGTrans.pos.x + borderPaddingX;
		float startY = inventoryBGTrans.pos.y - borderPaddingY;

		// Initialize Ability Icons at bottom showcasing the list of ability player has in its inventory
		for (int i = 0; i < inventoryMaxCount; ++i) {
			Button& btn = inventoryAbilityButton[i];

			// Set button x and y position
			float x = startX + (space * i) + (btnSizeX * i);
			float y = startY;

			btn = Button{ TextObject{"", 0.5f},
				Utils::AnchorMode::BOTTOM_CENTER, AEVec2{ x, y },
				AEVec2{ btnSizeX, btnSizeY }, Color{1,1,1}, AssetsManager::squareTexture
			};

			btn.SetTextOffset(0, 5);
		}

		// Initialize Ability Selector
		abilitySelectorBG = UIManager::Panel{ {1400.0f, 700.0f}, Color(0.6196f, 0.6196f, 0.6196f), Utils::AnchorMode::CENTER };

		AEVec2 cardSize{ CARD_WIDTH, CARD_HEIGHT }; // width and height
		float gap = (1400.0f - (cardSize.x * abilitiesCards.size())) / (abilitiesCards.size() + 1);
		AEVec2 cardPos{ -cardSize.x * 1.5f - gap, cardSize.y / 2.0f };

		for (int i = 0; i < selectorCount; ++i) {
			Card& card = abilitySelectorCards[i];
			card = Card{ "", cardPos, Color(1,0,0), AssetsManager::squareTexture, nullptr, nullptr };
			cardPos.x += cardSize.x + gap;

			// Set button text object anchor position
			TextObject& textObj = card.GetButton().GetTextObject();
			textObj.SetPosition(Utils::AnchorMode::CENTER, card.GetButton().GetTransform());
			textObj.SetOffsetPosition(0, -25.0f);
		}
	}

	void AbilityController::AbilitySelectorUI() {
		abilitySelectorBG.Draw();

		for (int i = 0; i < abilitiesCards.size(); ++i) {
			Ability const& ability = abilitiesCards[i];
			AEGfxTexture* imageTexture = ability.GetTexture() == "" ? AssetsManager::squareTexture : AssetsManager::GetTexture(ability.GetTexture().c_str());
			Card& card = abilitySelectorCards[i];
			// Update onClick and texture every frame
			card.SetTexture(imageTexture);

			Button& button = card.GetButton();
			button.SetOnClick([this, ability]() {
				OnClickAbilityCard(ability);
				});
			button.GetTextObject().TextContent(ability.GetName()); // update text content

			// Update and draw
			card.Update();
			card.Draw();
		}
	}

	void AbilityController::UpdateAbilityUI() {
		inventoryBackground.Draw();

		// Checking if player is hovering over UI
		LevelManager::CurrentLevel()->SetHoveringOverUI(CollisionManager::IsPointInRect(
			inventoryBackground.Pos().x, inventoryBackground.Pos().y, inventoryBackground.Size().x, inventoryBackground.Size().y));

		for (int i = 0; i < inventoryMaxCount; ++i) {
			Button& btn = inventoryAbilityButton[i];

			std::string iconName = "Empty";
			Color iconColor{ 0.5f, 0.5f, 0.5f };

			if (i < inventoryCount) {
				Ability* ability = FindAbility(inventoryAbilityIds[i]);
				iconName = ability->GetName();
				iconColor = ability->GetType() == AbilityType::OFFENSIVE ? Color(1, 0, 0) : Color(0, 0, 1);
				// Highlight current ability
				if (currentAbility == ability && currentInventoryId == i) {
					iconColor = Color(0, 0, 0);
				}
				else { 
					// Can only select ability that is not current selected
					btn.SetOnClick([this, ability, i]() {
						ChangeAbility(ability->Id());
						currentInventoryId = i;
						});
				}
			}
			else {
				// Empty slot — clear onclick so it does nothing
				btn.SetOnClick(nullptr);
			}

			btn.SetColor(iconColor);
			btn.GetTextObject().TextContent(iconName);
			if (!isSelectingAbility) {
				btn.Update();
			}
			btn.Draw();
		}
	}

	void AbilityController::PrintAllAbilities() {
		std::cout << "=================== Loaded " + controllerName + "==================\n";
		for (Ability const& ability : statsAbilities) {
			std::cout << "[AbilitiesManager] Loaded stats ability " << ability.Id() << " : " << ability.GetName() << std::endl;
		}
		std::cout << "[AbilitiesManager] Successfully loaded " << statsAbilities.size() << " stats abilities." << std::endl;

		for (Ability const& ability : offensiveAbilities) {
			std::cout << "[AbilitiesManager] Loaded offensive ability " << ability.Id() << " : " << ability.GetName() << std::endl;
		}
		std::cout << "[AbilitiesManager] Successfully loaded " << offensiveAbilities.size() << " offensive abilities." << std::endl;

		for (Ability const& ability : defensiveAbilities) {
			std::cout << "[AbilitiesManager] Loaded defensive ability " << ability.Id() << " : " << ability.GetName() << std::endl;
		}
		std::cout << "[AbilitiesManager] Successfully loaded " << defensiveAbilities.size() << " defensive abilities." << std::endl;
		std::cout << "=======================================================================\n";
	}

	void AbilityController::PrintInventory() {
		std::cout << "==============" + controllerName + " Inventory Count:" + std::to_string(inventoryCount) + "=============\n";
		for (int id : inventoryAbilityIds) {
			std::cout << std::setw(15) << std::left << "Id: " << id << " Name: " << FindAbility(id)->GetName() << '\n';
		}
	}

	void AbilityController::PrintInventoryCount() {
		std::cout << "==============" + controllerName + " Inventory Count" + "=============\n";
		for (int i = 0; i < inventoryTypeCount.size(); ++i) {
			std::string label;
			switch (i) {
			case 0:
				label = "Stats:";
				break;
			case 1:
				label = "Offensive:";
				break;
			case 2:
				label = "Defensive:";
				break;
			}
			std::cout << std::setw(15) << std::left << label << inventoryTypeCount[i] << '\n';
		}
	}
}

//// Change the current projectile based on the projectile ID
//void AbilityController::ChangeProjectile(s32 _projectileID) {
//	for (size_t i = 0; i < offensiveAbilities.size(); ++i) {
//		if (offensiveAbilities[i].GetProj().projectileId == _projectileID) {
//			ProjectileSelected = &offensiveAbilities[i];
//			std::cout << "[AbilitiesManager] Switched to: " << ProjectileSelected->GetName() << std::endl;
//			return;
//		}
//	}
//	std::cerr << "[AbilitiesManager] ERROR: Projectile ID " << _projectileID << " not found in offensive list!" << std::endl;
//}
//
//// Use the currently selected projectile if it has unlimited or any charges remaining
//bool AbilityController::UseProjectile() {
//	if (ProjectileSelected == nullptr) return false;
//
//	auto projData = ProjectileSelected->GetProj();
//
//	if (!projData.unlimited_charges) {
//		if (projData.charges <= 0) {
//			std::cout << "Out of charges for " << ProjectileSelected->GetName() << "!" << std::endl;
//			return false;
//		}
//
//		ProjectileSelected->UpdateCharges(-1);
//	}
//
//	std::cout << "Used " << ProjectileSelected->GetName() << ". Charges left: " << ProjectileSelected->GetProj().charges << std::endl;
//
//	return true;
//}
//
//// Use the currently selected projectile for the AI if it has unlimited or any charges remaining
//bool AbilityController::AIUseProjectile() {
//	if (AIProjectileSelected == nullptr) return false;
//
//	auto projData = AIProjectileSelected->GetProj();
//
//	if (!projData.unlimited_charges) {
//		if (projData.charges <= 0) {
//			std::cout << "Out of charges for " << AIProjectileSelected->GetName() << "!" << std::endl;
//			return false;
//		}
//
//		AIProjectileSelected->UpdateCharges(-1);
//	}
//
//	std::cout << "Used " << AIProjectileSelected->GetName() << ". Charges left: " << AIProjectileSelected->GetProj().charges << std::endl;
//
//	return true;
//}