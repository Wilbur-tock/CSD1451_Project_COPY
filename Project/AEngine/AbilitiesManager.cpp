/*
Authors: Dayrius, Justin, Wilbur
Description: Store the data of the player and enemy ability controllers which contains
ability data informatiton about the player and enemy. Functions to control and manage the ability
data.

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "AbilitiesManager.hpp"

#include "AbilitiesFileManager.hpp"
#include "GameTrackingManager.hpp"

#include "Utils.hpp"
#include "Color.hpp"
#include "UIManager.hpp"
#include "Card.hpp"
#include "AssetsManager.hpp"
#include "CameraManager.hpp"
#include "CollisionManager.hpp"
#include "LevelManager.hpp"
#include "AudioManager.hpp"

#include "Tutorial.hpp"

namespace AbilitiesManager {
	AbilityController playerAbilityController;
	AbilityController enemyAbilityController;

	// Load all abilities from the JSON file and categorize them into their respective vectors
	void AbilityController::LoadAllAbilities(bool isPlayer) {
		// Reset data in the controller
		offensiveAbilities.clear();
		statsAbilities.clear();
		defensiveAbilities.clear();

		allAbilities.clear();
		inventoryIds.clear();
		abilityModifiers.clear();

		isPlayerControlled = isPlayer;

		// Reading of data from json file
		nlohmann::json data;
		AbilitiesFileManager::ReadJsonFile(data, "Abilities.json");

		// Read enemies ONLY ability and merge it to the data
		if (isPlayer) {
			controllerName = "Player Ability Controller";
			//ReadAbilitiesFromFile(data, "PlayerAbilities.json");
		}
		else {
			controllerName = "Enemy Ability Controller";
			//ReadAbilitiesFromFile(data, "AiAbilities.json");
		}

		totalAbilityCount = data.size();
		size_t reserveCount = totalAbilityCount;

		// Reserve space for all vector used in these controller
		projectileAbilities.reserve(reserveCount);
		allAbilities.reserve(reserveCount);
		offensiveAbilities.reserve(reserveCount);
		statsAbilities.reserve(reserveCount);
		defensiveAbilities.reserve(reserveCount);
		abilityModifiers.reserve(Config::MAX_ABILITY_MODIFIER_COUNT);

		inventoryIds.clear();
		inventoryIds.reserve(Config::MAX_INVENTORY_COUNT);
		inventoryTypeCount.clear();
		inventoryTypeCount.resize(static_cast<int>(AbilityType::COUNT));

		int abilityId = 0;
		for (auto& item : data) { // Your JSON is an array at the root
			// Reading of json data files
			Ability newAbility = AbilitiesFileManager::ReadAbilityInfo(item);
			newAbility.Id(abilityId);

			AbilitiesFileManager::ReadAllStats(newAbility, item);

			// Add to the type of ability vector
			switch (newAbility.GetType()) {
			case AbilityType::STATS:
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

			// checking if the this ability projectileId is a new id that has not been added to the allProjectilesAbilities vector
			bool addToProjectilesAbilities = true;
			for (Ability const& ability : projectileAbilities) {
				int projectileId = ability.GetProjectile().projectileId; 
				if (projectileId == newAbility.GetProjectile().projectileId) { // already in the vector don't add
					addToProjectilesAbilities = false;
					break;
				}
			}

			// Only add each projectileId once for example if there is like 5 abilites that uses projectileId (0) 
			// only add into the vector once only
			if (addToProjectilesAbilities) {
				projectileAbilities.push_back(newAbility);
				//std::cout << "[AbiltiesManager] Projectile Ability: " << newAbility.GetName() << '\n';
			}

			// Increment ability id after initalizing everything
			++abilityId;
		}

		LoadStartingAbilities();
		//PrintAllAbilities();
		//PrintInventory();
		//RandomiserTest(50, 0);
	}

	// Load the starting abilities for the player and AI
	void AbilityController::LoadStartingAbilities() {
		AddAbility(0); // Always add default ability for both player and enemy
		// Just randomise the max inventory count - 2 (because of the default ability and leave 1 empty for player/enemy to select)
		int randomCount = Config::MAX_INVENTORY_COUNT - 2;
		InventoryRandomiser(randomCount);

		Reset();

		// Add default at the last slot if it in testing mode
		if (testingMode) {
			AddAbility(0);
			ChangeCurrentAbility(Config::MAX_INVENTORY_COUNT - 1);
			std::cout << inventoryIds.size() << '\n';
		}

		//std::cout << "[AbilitiesManager] List of starting abilities:" << '\n';
		//for (Ability* ability : inventory) {
		//	std::cout << ability->GetName() << " Charges: " << ability->AbilityCharges() << '\n';
		//}
	}

	void AbilityController::Reset() {
		isAbilityTriggered = false;

		if (testingMode) return;
		// Always reset back to default ability
		ChangeCurrentAbility(0);
		currentInventoryId = 0;
	}

	void AbilityController::SetAIAbilitySelection(int difficulty) {
		float bronzeBoost = 1.0f, sliverBoost = 1.0f, goldBoost = 1.0f, legendaryBoost = 1.0f;
		if (difficulty == 0) {
			bronzeBoost = 5.0f;
			sliverBoost = 0.75f;
			goldBoost = 0.50f;
			legendaryBoost = 0.1f;
		}
		else if (difficulty == 1) {
			sliverBoost = 4.0f;
		}
		else if (difficulty == 2) {
			bronzeBoost = 0.25f;
			sliverBoost = 0.5f;
			goldBoost = 2.5f;
			legendaryBoost = 2.5f;
		}

		IncreaseAbilityChance(Tier::BRONZE, bronzeBoost);
		IncreaseAbilityChance(Tier::SLIVER, sliverBoost);
		IncreaseAbilityChance(Tier::GOLD, goldBoost);
		IncreaseAbilityChance(Tier::LEGENDARY, legendaryBoost);
	}

	void AbilityController::AISelectAbility() {
		if (hasEnemySelectedAbility) return;

		InventoryRandomiser(1); // randomly select a new ability to add to the inventory

		// Select a random inventory ability
		int randomInventoryId = Utils::RandomValue(0, InventoryCount());

		ChangeCurrentAbility(randomInventoryId);
		hasEnemySelectedAbility = true;

		std::cout << "[AbilitiesManager] Enemy selected ability: " << GetCurrentAbility().GetName() << ' ' << currentInventoryId << '\n';
	}

	void AbilityController::InventoryRandomiser(int count) {
		bool canRandomiseSameAbility = true; // can randomise the same ability
		std::vector<int> previousIds;

		for (int i = 0; i < count; ++i) {
			std::vector<Ability> pool = (OffensiveOrDefensive() == AbilityType::OFFENSIVE) ? offensiveAbilities : defensiveAbilities;

			// Clean the ability pool of already selected abilities and default ability
			pool.erase(std::remove_if(pool.begin(), pool.end(), [&](Ability const& a) {
				if(a.Id() == 0) return true;

				if (!canRandomiseSameAbility) {
					for (int prevId : previousIds) {
						if (prevId == a.Id()) return true;
					}
				}

				return false;

				}), pool.end()
			);

			int selectedId = RandomAbilityId(pool);
			previousIds.push_back(selectedId);
			AddAbility(selectedId); // add to inventory
		}
	}

	// Randomly select abilities for the ability selector UI
	void AbilityController::AbilitySelectionRandomiser() {
		abilitiesCards.clear();

		// Randomise Abilties
		int count = Config::MAX_ABILITY_SELECTOR_COUNT;
		// if player only uses default ability they are able to choose a stats ability
		if (InventoryCount() >= Config::MAX_INVENTORY_COUNT) {
			for (int i = 0; i < count; ++i) {
				abilitiesCards.push_back(*GetAbility(RandomAbilityId(statsAbilities)));
			}
		}
		else {
			// out of the 3 card at least should have 1 offensive and defensive ability
			int offensiveCount = 1;
			int defensiveCount = 1;
			// find which ever type has the lesser in its inventory
			OffensiveOrDefensive() == AbilityType::OFFENSIVE ? ++offensiveCount : ++defensiveCount;

			for (int i = 0; i < offensiveCount; ++i) {
				abilitiesCards.push_back(*GetAbility(RandomAbilityId(offensiveAbilities)));
			}
			for (int i = 0; i < defensiveCount; ++i) {
				abilitiesCards.push_back(*GetAbility(RandomAbilityId(defensiveAbilities)));
			}
		}
	}

	int AbilityController::RandomAbilityId(std::vector<Ability> const& abilities) {
		// Calculate sum of all chances
		float totalChance = 0;
		for (Ability const& ability : abilities) {
			totalChance += ability.Chance();
		}

		float roll = Utils::RandomValue(0.0f, 1.0f);
		float cumulativePercentage = 0;

		for (Ability const& ability : abilities) {
			float normalizedChance = ability.Chance() / totalChance;
			cumulativePercentage += normalizedChance;
			if (roll <= cumulativePercentage) {
				//std::cout << "Random Ability: " << ability.GetName() << " Tier: " << std::to_string(static_cast<int>(ability.GetTier())) << '\n';
				int id = ability.Id();
				if (id == 0) return 1; // if chosen ability is default return 1
				return id;
			}
		}

		return 0; // return default ability should not happen!
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
		else {
			return offensiveCount < defensiveCount ? AbilityType::OFFENSIVE : AbilityType::DEFENSIVE;
		}
	}

	void AbilityController::IncreaseAbilityChance(Tier tier, float increasePercentage) {
		if (increasePercentage == 1.0f) return; // 1.0f is not an increase return

		for (Ability& ability : offensiveAbilities) {
			if (ability.GetTier() == tier) {
				ability.Chance(increasePercentage);
			}
		}

		for (Ability& ability : defensiveAbilities) {
			if (ability.GetTier() == tier) {
				ability.Chance(increasePercentage);
			}
		}

		for (Ability& ability : allAbilities) {
			if (ability.GetTier() == tier) {
				ability.Chance(increasePercentage);
			}
		}
	}

	void AbilityController::AbilityEffects() {
		Ability const& ability = GetCurrentAbility();
		std::string abilityName = GetCurrentAbility().GetName();
		Level* level = LevelManager::CurrentLevel();

		// Activate a ability effect here
		if (abilityName == "Tower Shield") {
			AudioManager::PlayInstantAbilitySFX(abilityName);
			level->GetTowerShield(isPlayerControlled).SetShieldOn(
				GetCurrentAbility().GetStats().shield,
				GetCurrentAbility().GetStatus().rounds
			);
		}
		else if (abilityName == "Rebuild") {
			AudioManager::PlayInstantAbilitySFX(abilityName);
			level->ToggleTerritoryTiles(isPlayerControlled, true);
		}
		else if (abilityName == "Extra Shot") {
			AudioManager::PlayInstantAbilitySFX(abilityName);
			level->SetExtraTurn();
		}
		else if (abilityName == "Skip Turn") {
			AudioManager::PlayInstantAbilitySFX(abilityName);
			level->SetSkipOpponentTurn();
		}
		else if (abilityName == "Invisibility") {
			AudioManager::PlayInstantAbilitySFX(abilityName);
			for (Character& character : LevelManager::CurrentLevel()->GetCharacters(isPlayerControlled)) {
				character.attribute.SetInvisibility(ability.GetStatus().rounds);
			}
		}
		else if (abilityName == "Cleanse") {
			AudioManager::PlayInstantAbilitySFX(abilityName);
			for (Character& character : LevelManager::CurrentLevel()->GetCharacters(isPlayerControlled)) {
				character.attribute.CleanseNegativeEffects();
			}
		}

		UseAbilityWithModifier(ability);
		//PrintAbilityModifiers();
	}

	void AbilityController::UseAbilityWithModifier(Ability const& ability) {
		// Add modifier after shooter has shot out the projectile if it a modifier
		if (ability.GetStatus().targetSelf) {
			// Add modifier to self
			AddModifier(ability);
		}
		else {
			// Add modifier to the opponent
			isPlayerControlled ? enemyAbilityController.AddModifier(ability) : playerAbilityController.AddModifier(ability);
		}
	}

	void AbilityController::AddModifier(Ability const& ability) {
		if (!ability.IsAbilityModifier()) return;

		// Remove the exisiting modifier if it the same as the current ability
		abilityModifiers.erase(
			std::remove_if(abilityModifiers.begin(), abilityModifiers.end(),
				[ability](Ability const& ab) {
					return ab.Id() == ability.Id();
				}),
			abilityModifiers.end()
		);

		// push_back a copy of the currentAbility if the effects last more than 1 round
		abilityModifiers.push_back(ability);

		// if the caster does not start the round, and added a modifier that last for more than 1 round 
		// it should not decrease immediately after its turn end but after the next round end
		if (LevelManager::CurrentLevel()->IsStarting(isPlayerControlled) && !ability.GetStatus().isPositive) {
			abilityModifiers.back().JustAddModifier(true);
		}

		//std::cout << "Added modifier: " << ability.GetName() << '\n';
	}

	void AbilityController::UpdateModifiers() {
		//PrintAbilityModifiers();
		for (Ability& ability : abilityModifiers) {
			// Update lifespan
			ability.UpdateRoundLifespan();
		}
		CheckModifiers();
		//PrintAbilityModifiers();
	}

	void AbilityController::CheckModifiers() {
		// Remove modifier from vector containing all the active ability modifiers if either turns attribute or shield attribute reaches 0
		abilityModifiers.erase(
			std::remove_if(abilityModifiers.begin(), abilityModifiers.end(),
				[](Ability& ability) {
					return ability.GetStatus().rounds <= 0 || (ability.GetName() == "Tower Shield" == ability.GetStats().shield <= 0);
				}),
			abilityModifiers.end()
		);
	}

	void AbilityController::ChangeCurrentAbility(int newInventoryId) {
		if (newInventoryId < 0 || newInventoryId >= Config::MAX_INVENTORY_COUNT) newInventoryId = 0;
		// Change current inventory id
		currentInventoryId = newInventoryId;
		int abilityId = inventoryIds[newInventoryId];

		// update current ability id and vector id
		for (int i = 0; i < allAbilities.size(); ++i) {
			// Found the same ability id in allAbilitiesVector return if found
			if (allAbilities[i].Id() == abilityId) {
				currentAbilityVectorId = i;
				return;
			}
		}

		currentAbilityVectorId = 0;
		std::cerr << "Can't find ability id: " << std::to_string(abilityId) + ' ' + controllerName + ' ' + std::to_string(allAbilities.size()) << '\n';
	}

	// Use the currently select ability if it has unlimited or any charges remaining
	void AbilityController::UseAbility() {
		if (GetCurrentAbility().GetProjectile().unlimitedCharges) return;
		if (GetCurrentAbility().AbilityCharges() <= 0) return; // has no ability charges can't use this ability return false without updating the inventory

		AbilityEffects();
		GameTrackingManager::gameData.AddAbilityUsed(isPlayerControlled, static_cast<int>(GetCurrentAbility().GetType()));

		if (!testingMode) {
			// Minus ability charges from current ability and remove from inventory
			RemoveAbility(SetCurrentAbility());
		}

		std::cout << "[AbilitiesManager] Ability used: " + GetCurrentAbility().GetName()
			+ " Charges Remaining: " + std::to_string(GetCurrentAbility().AbilityCharges()) << '\n';
	}

	// Add ability charges from the based on selection
	void AbilityController::AddAbility(int abilityId, int chargesToAdd) {
		// Don't add any new ability after inventory count has reach the max
		if (IsInventoryFull()) {
			std::cout << "[AbilitesManager] Inventory is full can't add any new ability!" << '\n';
			return; 
		}
		Ability* found = GetAbility(abilityId);
		if (!found) {
			std::cerr << "[AbilitiesManager] Can't find to add Ability Id: " + std::to_string(abilityId) << '\n';
			return;
		}
		// Store the reference of the found ability and add the charges 
		Ability& ability = *found;
		ability.AbilityCharges(chargesToAdd);
		UpdateInventory(ability, true);
	}

	void AbilityController::RemoveAbility(Ability& ability) {
		ability.AbilityCharges(-1);
		UpdateInventory(ability, false);
	}

	// Helper function to add or remove abilities from the inventory vector based on the ability charges
	void AbilityController::UpdateInventory(Ability& ability, bool toAddToVector) {
		int typeId = static_cast<int>(ability.GetType()) - 1;
		if (typeId < 0 || typeId > 3) {
			std::cerr << "Input Wrong Type Id (0-2): " + ability.GetName() + ' ' + std::to_string(static_cast<int>(ability.GetType())) << '\n';
		}

		if (toAddToVector) {
			std::cout << "[AbilitiesManager] Added " << ability << " into inventory" << '\n';
			// update count
			++inventoryTypeCount[typeId];
			// Add to inventoryAbilityIds list
			inventoryIds.push_back(ability.Id());
		}
		else {
			std::cout << "[AbilitiesManager] Removed " << ability << " from inventory" << '\n';
			// update count
			--inventoryTypeCount[typeId];
			// Remove from inventoryAbilityIds vector
			inventoryIds.erase(inventoryIds.begin() + currentInventoryId);
		}
	}

	Ability* AbilityController::GetAbility(int abilityId) {
		for (Ability& ab : allAbilities) {
			if (ab.Id() == abilityId) {
				return &ab;
			}
		}

		std::cerr << "[AbilitiesManager] Can't find id: " + std::to_string(abilityId) + " in the abilities list\n";
		return 0;
	}

	Ability* AbilityController::GetAbility(std::string abilityName) {
		for (Ability& ab : allAbilities) {
			if (ab.GetName() == abilityName) {
				return &ab;
			}
		}

		std::cerr << "[AbilitiesManager] Can't find " + abilityName + " in the ability modifier vector\n";
		return nullptr;
	}

	Ability* AbilityController::GetAbilityModifiers(std::string abilityName) {
		for (Ability& ab : abilityModifiers) {
			if (ab.GetName() == abilityName) {
				return &ab;
			}
		}

		std::cerr << "[AbilitiesManager] Can't find " + abilityName + " in the ability modifier vector\n";
		return nullptr;
	}

	// Debug commands true to toggle through all abilities or false to toggle through of all the active abilities this controller has in its inventory
	void AbilityController::ToggleAbility(bool isAllAbilities, int addValue) {
		if (isAllAbilities) {
			int abilityId = GetCurrentAbility().Id();
			abilityId += addValue;
			Utils::Wrap(abilityId, 0, static_cast<int>(allAbilities.size()-1));

			// always use the last index if trying to use an ability not inside the inventory
			currentInventoryId = Config::MAX_INVENTORY_COUNT - 1;
			if (currentInventoryId < inventoryIds.size()) { // last slot is not empty
				RemoveAbility(*GetAbility(inventoryIds[currentInventoryId]));
			}

			AddAbility(abilityId);

			if (GetCurrentAbility().GetType() == AbilityType::STATS) {
				UseStatsAbility(GetCurrentAbility());
			}
		}
		else { // Toggle through inventory abilities
			currentInventoryId += addValue;
			Utils::Wrap(currentInventoryId, 0, Config::MAX_INVENTORY_COUNT - 1);
		}

		ChangeCurrentAbility(currentInventoryId);
	}

	// Display the ability selector UI for the player to select an ability
	void AbilityController::UpdateAbilitySelector() {
		if (!isSelectingAbility) return;

		//std::cout << "Player Selecting Abilities:\n";
		CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

		DrawAbilitySelectorUI();
	}

	void AbilityController::OnClickAbilityCard(Ability const& ability) {
		Tutorial::SetTutorialMsg(2);

		if (ability.GetType() != AbilityType::STATS) {
			AddAbility(ability.Id());
		}
		else {
			UseStatsAbility(ability);
		}

		isSelectingAbility = false;
		CameraManager::camera.SetState(CameraManager::CameraController::INPUT);
	}

	void AbilityController::UseStatsAbility(Ability const& ability) {
		statsAbility = ability;
		std::string abilityName = ability.GetName();

		if (abilityName == "Max Heal" || abilityName == "Major Heal" || abilityName == "Minor Heal") {
			for (Character& character : LevelManager::CurrentLevel()->GetCharacters(isPlayerControlled)) {
				character.attribute.Heal(static_cast<float>(ability.GetStats().hp / 100.0f));
			}
		}
		else if (abilityName == "Invincibility" || abilityName == "Attack Boost" || abilityName == "Defense Boost") {
			isStatsAbilityActive = true;
		}

		//keep only if the sound is a short "buff activate" cue
		AudioManager::PlayInstantAbilitySFX(abilityName);
		UseAbilityWithModifier(ability);
	}

	void AbilityController::ResetStatsBoost() {
		statsAbility.ResetStats();
		isStatsAbilityActive = false;
	}

	// UI OBJECTS
	UIManager::Panel inventoryBackground;
	Button inventoryAbilityButton[Config::MAX_INVENTORY_COUNT];
	UIManager::PanelText abilityTooltip;

	// Selecting Ability UI
	UIManager::Panel abilitySelectorBG;
	Card abilitySelectorCards[Config::MAX_ABILITY_SELECTOR_COUNT];

	Color statsColor{ 0.75f,0.75f,0 }, offensiveColor{0.75f,0,0}, defensiveColor{0,0,0.75f};
	Color bgPanelColor{ 0.6196f, 0.6196f, 0.6196f };

	void AbilityController::InitializeAllAbilityUI() {
		inventoryBackground = UIManager::Panel{ {750.0f, 100.0f}, bgPanelColor, Utils::AnchorMode::BOTTOM_CENTER};

		Transform inventoryBGTrans = inventoryBackground.GetTransform();
		float totalWidth = inventoryBGTrans.size.x;

		// Values to edit the button size
		float space = 15.0f;
		float borderPaddingX = 25.0f;
		float borderPaddingY = 5.0f;

		// Calculate total width of spaces between buttons
		// [BTN][SPACE][BTN][SPACE].....
		float spaceTotalWidth = space * (Config::MAX_INVENTORY_COUNT - 1);
		float totalBtnWidth = totalWidth - spaceTotalWidth - (borderPaddingX * 2.0f);

		float btnSizeX = totalBtnWidth / Config::MAX_INVENTORY_COUNT;
		float btnSizeY = inventoryBGTrans.size.y - (borderPaddingY * 2.0f);

		// Set starting position
		float startX = inventoryBGTrans.pos.x + borderPaddingX;
		float startY = inventoryBGTrans.pos.y - borderPaddingY;

		AEVec2 inventoryInfoPanelPos{ 350,150 };

		abilityTooltip = UIManager::PanelText{
			UIManager::Panel{inventoryInfoPanelPos, PanelColor(), Utils::AnchorMode::BOTTOM_RIGHT},
			TextObject{"", 0.7f, Color{0,0,0}, Transform{{0,0}, {inventoryInfoPanelPos.x - 10,inventoryInfoPanelPos.y - 10}} }
		};
		//inventoryInfoPanel.AddOffsetPosition({ 0, -75.0f });
		
		// get card texture
		AEGfxTexture* cardTexture = AssetsManager::GetTexture("Assets/card_texture.png");

		// Initialize Ability Icons at bottom showcasing the list of ability they have in its inventory
		for (int i = 0; i < Config::MAX_INVENTORY_COUNT; ++i) {
			Button& btn = inventoryAbilityButton[i];

			// Set button x and y position
			float x = startX + (space * i) + (btnSizeX * i);
			float y = startY;

			btn = Button{ TextObject{"", 0.5f},
				Utils::AnchorMode::BOTTOM_CENTER, AEVec2{ x, y },
				AEVec2{ btnSizeX, btnSizeY }, Color{1,1,1}, AssetsManager::squareTexture
			};
			btn.SetIconGaps({ 15,15,5,20 });
			//btn.SetTextOffset(0, 5);
		}

		// Initialize Ability Selector
		abilitySelectorBG = UIManager::Panel{ {1400.0f, 700.0f}, bgPanelColor.EditAlpha(0.8f), Utils::AnchorMode::CENTER };

		AEVec2 cardSize{ CARD_WIDTH, CARD_HEIGHT }; // width and height
		float gap = (1400.0f - (cardSize.x * abilitiesCards.size())) / (abilitiesCards.size() + 1);
		AEVec2 cardPos{ -cardSize.x * 1.5f - gap, cardSize.y / 2.0f };

		for (int i = 0; i < Config::MAX_ABILITY_SELECTOR_COUNT; ++i) {
			Card& card = abilitySelectorCards[i];
			card = Card{ "", cardPos, Color(1,0,0), cardTexture, nullptr, nullptr };
			cardPos.x += cardSize.x + gap; // update pos for the next card in the for loop
		}
	}

	void AbilityController::DrawAbilitySelectorUI() {
		abilitySelectorBG.Draw();

		for (int i = 0; i < abilitiesCards.size(); ++i) {
			Ability const& ability = abilitiesCards[i];
			Card& card = abilitySelectorCards[i];

			// Update onClick and texture every frame
			card.SetIconTexture(ability.GetTexture());
			card.SetBGColor(ability.GetType() == AbilityType::STATS ? statsColor : 
				ability.GetType() == AbilityType::OFFENSIVE ? offensiveColor : defensiveColor);

			Button& button = card.GetButton();
			button.SetOnClick([this, ability]() {
				OnClickAbilityCard(ability);
				});
			card.GetTitle().TextContent(ability.GetName()); // update title
			button.GetTextObject().TextContent(ability.GetDescriptionWithStats()); // update description

			// Update and draw
			card.Update();
			card.Draw();
		}
	}

	void AbilityController::DrawAbilityInventoryUI() {
		inventoryBackground.Draw();

		// Checking if player is hovering over inventory UI if it is disable shooting
		LevelManager::CurrentLevel()->SetInventoryHoveringUI(CollisionManager::IsPointInRect(
			inventoryBackground.GetTransform().pos.x, inventoryBackground.GetTransform().pos.y, inventoryBackground.GetTransform().size.x, inventoryBackground.GetTransform().size.y));

		// Draw inventory UI
		for (int i = 0; i < Config::MAX_INVENTORY_COUNT; ++i) {
			Button& btn = inventoryAbilityButton[i];

			std::string iconName = "Empty";
			Color iconColor{ 0.5f, 0.5f, 0.5f };

			if (i < InventoryCount()) {
				int inventoryId = testingMode ? static_cast<int>(inventoryIds.size() - 1) : i;
				Ability* ability = GetAbility(inventoryIds[inventoryId]);

				iconName = ability->GetName();
				iconColor = ability->GetType() == AbilityType::OFFENSIVE ? offensiveColor : defensiveColor;
				// Highlight current ability
				if (GetCurrentAbility().Id() == ability->Id() && currentInventoryId == i) {
					iconColor = Color{ 0, 0, 0 };
				}
				else {
					// Can only select ability that is not current selected
					btn.SetOnClick([this, inventoryId]() {
						Tutorial::SetTutorialMsg(3);
						ChangeCurrentAbility(inventoryId);;
					});
				}

				// Update icon
				btn.SetIcon(ability->GetTexture());

				if (btn.IsHovered()) {
					abilityTooltip.Update(ability->GetDescriptionWithStats());
					abilityTooltip.Draw();
				}
			}
			else {
				// Empty slot — clear onclick so it does nothing
				btn.SetIcon(nullptr);
				btn.SetOnClick(nullptr);
			}

			// Update textContent and color
			btn.SetColor(iconColor);
			btn.GetTextObject().TextContent(iconName);
			// Only check for inputs if it player controlled
			if (isPlayerControlled && !isSelectingAbility) {
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
		std::cout << "==============" + controllerName + " Inventory Count:" + std::to_string(InventoryCount()) + "=============\n";
		for (int id : inventoryIds) {
			Ability const& ability = *GetAbility(id);
			std::cout << std::setw(5) << std::left << "Id: " + std::to_string(id) << " Name: " + ability.GetName()
				+ " Charges: " + std::to_string(ability.AbilityCharges()) << '\n';
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

	void AbilityController::PrintAbilityModifiers() {
		std::cout << "==============" + controllerName + " Ability Modifiers" + "=============\n";
		for (Ability const& ability : abilityModifiers) {
			std::cout << std::setw(15) << std::left << ability.GetName()
				<< "Remaining Turns " + std::to_string(ability.GetStatus().rounds) << ' '
				<< std::boolalpha << ability.JustAddModifier() << '\n';
		}
	}

	void AbilityController::RandomiserTest(int count, int levelDifficulty) {
		if (!isPlayerControlled) {
			SetAIAbilitySelection(levelDifficulty);
			int tierCount[] = { 0,0,0,0 };
			for (int i = 0; i < count; ++i) {
				Ability ability = offensiveAbilities[RandomAbilityId(offensiveAbilities)];
				++tierCount[static_cast<int>(ability.GetTier()) - 1];
				std::cout << "Tier: " << std::to_string(static_cast<int>(ability.GetTier())) << ' ' << ability.GetName() << '\n';
			}
			std::cout << "Tier Count: " << tierCount[0] << ' ' << tierCount[1] << ' ' << tierCount[2] << ' ' << tierCount[3] << ' ' << '\n';
		}
	}
}