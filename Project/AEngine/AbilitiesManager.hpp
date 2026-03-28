#ifndef ABILITIESMANAGER_HPP
#define ABILITIESMANAGER_HPP

#include "pch.hpp"
#include "Ability.hpp"

namespace AbilitiesManager {
	class AbilityController {
	public: // Getters and Setters
		std::vector<Ability> const& GetOffensiveAbilities() { return offensiveAbilities; }
		std::vector<Ability> const& GetStatsAbilities() { return statsAbilities; }
		std::vector<Ability> const& GetDefensiveAbilities() { return defensiveAbilities; }

		std::vector<Ability> const& GetProjectilesAbilties() { return projectileAbilities; }
		std::vector<Ability> const& GetAbilityModifiers() { return abilityModifiers; }

		Ability const& GetCurrentAbility() { return allAbilities[currentAbilityVectorId]; }

		int InventoryCount() const { return static_cast<int>(inventoryIds.size()); }
		int CurrentInventoryId() const { return currentInventoryId; }
		std::vector<int> const& InventoryIds() const { return inventoryIds; }
		bool IsInventoryFull() const { return InventoryCount() >= Config::MAX_INVENTORY_COUNT; }

		bool IsSelectingAbility() const { return isSelectingAbility; }
		void IsSelectingAbility(bool _isSelectingAbility) { isSelectingAbility = _isSelectingAbility; }

		bool IsAbilityTriggered() const { return isAbilityTriggered; }
		void IsAbilityTriggered(bool isTriggered) { isAbilityTriggered = isTriggered; }

		// return the projectile Id to change the type of projectile the CANNON is currently using
		// if the ability type is an offensive which affects the projectile
		int GetProjectileId() { return GetCurrentAbility().GetProjectile().projectileId; }

		bool IsStatsAbilityActive() const { return isStatsAbilityActive; }
		Ability const& StatsAbility() const { return statsAbility; }

	private:
		Ability& SetCurrentAbility() { return allAbilities[currentAbilityVectorId]; }

	public: // Member functions
		// Load all different type of abilites to be called at the start of the game
		void LoadAllAbilities(bool isPlayer); //Loads all player abils from Abilities.json file
		//void LoadAllAIAbilities(); //Loads all AI abils from AiAbilities.json file
		void LoadStartingAbilities();

		void SetTestingMode() { testingMode = true; }
		// Reset to default ability if current ability if an offensive and charges remaining is 0 to 
		// ensure that an ability with 0 charges cannot be used again
		void Reset();

		// AI Member functions

		void ResetAIAbilitySelection() { hasEnemySelectedAbility = false; }
		// Set the chances of ability to increase or decrease based on difficulty level
		// With hard legendary and gold tier ability will have a increase chances of being selected by AI
		void SetAIAbilitySelection(int difficulty);
		// Randomly select an ability for the AI if it has not already selected one
		void AISelectAbility();

		// Randomise ability selection functions

		void AbilitySelectionRandomiser();
		// count the number of offensive and defensive ability in the inventory
		// add a random ability id based on the type which has the lower number offensive or defensive
		// to the inventory or card selection based on the given count input and boolean
		void InventoryRandomiser(int count);
		// Get a random ability ids from the abilities vector input 
		// the chances of pulling each ability is based on their ability tier
		int RandomAbilityId(std::vector<Ability> const& abilities);
		AbilityType OffensiveOrDefensive();
		// Increase chances of the tier chosen to appear more in selection 
		// affects allAbiltiies, offensiveAbiltiies and defensiveAbilities
		void IncreaseAbilityChance(Tier tier, float increasePercentage);

		// Activate ability effects that will be activated once UseAbility() is called
		void AbilityEffects();
		void UseAbilityWithModifier(Ability const& ability);
		void AddModifier(Ability const& ability);
		// Remove 1 round attribute from each ability modifier to be called in every new round
		void UpdateModifiers();
		void CheckModifiers();

		// Change the currentInventoryId and ability user is currently using
		void ChangeCurrentAbility(int newInventoryId);

		// Use the currently select ability if it has unlimited or any charges remaining
		// return true and the selected ability can be use, otherwise false
		void UseAbility();
		// Add ability charges from the based on selection
		void AddAbility(int abilityId, int chargesToAdd = 1);
		void RemoveAbility(Ability& ability);
		void UpdateInventory(Ability& ability, bool toAddToVector);

		// return a pointer to an ability in the AllAbilties vector based on the abilityId
		Ability* GetAbility(int abilityId);
		// return a pointer to an ability in the AllAbilties vector based on the abilityName given
		Ability* GetAbility(std::string abilityName);
		// return a pointer to an ability in the abilityModifiers vector based on the abilityName given
		Ability* GetAbilityModifiers(std::string abilityName);

		// True to toggle through all abilities or 
		// false to toggle through of all the inventory abilities this controller has
		void ToggleAbility(bool isAllAbilities, int addValue);

		void UpdateAbilitySelector();
		void OnClickAbilityCard(Ability const& ability);

		void UseStatsAbility(Ability const& ability);
		void ResetStatsBoost();

		// UI functions

		void InitializeAllAbilityUI();
		void DrawAbilitySelectorUI();
		// Draw ability inventory and checks for button input
		void DrawAbilityInventoryUI();

	private: // private member functions
		void PrintInventory();
		// Print all abilities that was loaded from the json into the console
		void PrintAllAbilities();
		void PrintInventoryCount();
		void PrintAbilityModifiers();
		void RandomiserTest(int count, int levelDifficulty);

	private:
		std::string controllerName; // use for debugging

		// These vector are used to randomise which ability can be selected

		std::vector<Ability> offensiveAbilities; //vector for offensive abils
		std::vector<Ability> statsAbilities; //vector for stats abils
		std::vector<Ability> defensiveAbilities; //vector for defensive abils

		size_t totalAbilityCount = 0;
		std::vector<Ability> allAbilities; // vector containing all the different type of abilities
		std::vector<Ability> projectileAbilities; // vector containing all the ability that uses it own unique projectile mode
		std::vector<Ability> abilityModifiers;// vector containing all the active abilites that last for more than 1 round (HP boost, attack boost, etc)

		int currentAbilityVectorId = 0; // vector id of the current ability stored in the allAbilities vector

		// ids of all the abilityId that is stored in the inventory 
		// for example inventory has 5 max space [0][0][1][2][3] < numbers representing the abilityId
		// has 2 charges of abilityId 2 in inventory while the rest 1 charges only
		std::vector<int> inventoryIds; 
		std::vector<int> inventoryTypeCount; // To keep track of how many of each ability type there is in the inventory
		int currentInventoryId = 0;

		bool isSelectingAbility = false;
		bool isAbilityTriggered = false;
		bool isPlayerControlled = true;

		bool hasEnemySelectedAbility = false; // has enemy selected their ability

		bool testingMode = false;

		bool isStatsAbilityActive = false;
		Ability statsAbility; // stats ability boost to apply onto caster when they shoot

		std::vector<Ability> abilitiesCards; // vector containing the 3 abilities to be displayed in the ability selector for the player to select from
	};

	extern AbilityController playerAbilityController;
	extern AbilityController enemyAbilityController;
}

#endif // !ABILITIESMANAGER_HPP
