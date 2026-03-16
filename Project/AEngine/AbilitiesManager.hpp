#ifndef ABILITIESMANAGER_HPP
#define ABILITIESMANAGER_HPP

#include "pch.hpp"
#include "Projectile.hpp"
#include "Ability.hpp"

namespace AbilitiesManager {
	class AbilityController {
	public:
		// Load all different type of abilites to be called at the start of the game
		void LoadAllAbilities(bool isPlayer); //Loads all player abils from Abilities.json file
		//void LoadAllAIAbilities(); //Loads all AI abils from AiAbilities.json file
		void LoadStartingAbilities();

		std::vector<Ability> const& GetOffensiveAbilities() { return offensiveAbilities; }
		std::vector<Ability> const& GetStatsAbilities() { return statsAbilities; }
		std::vector<Ability> const& GetDefensiveAbilities() { return defensiveAbilities; }

		std::vector<Ability> const& GetProjectilesAbilties() { return allProjectilesAbilties; }
		std::vector<Ability> const& GetAbilityModifiers() { return abilityModifiers; }

		Ability* CurrentAbility() const { return currentAbility; }
		int InventoryCount() const{ return inventoryCount; }

		bool IsSelectingAbility() const { return isSelectingAbility; }
		void IsSelectingAbility(bool _isSelectingAbility) { isSelectingAbility = _isSelectingAbility; }

		bool IsAbilityTriggered() const { return isAbilityTriggered; }
		void IsAbilityTriggered(bool isTriggered) { isAbilityTriggered = isTriggered; }

		std::vector<Ability*> const& Inventory() const { return inventory; }
		bool IsInventoryFull() const { return inventoryCount >= Config::MAX_INVENTORY_COUNT; }

		AbilityType OffensiveOrDefensive();

		// Activate ability effects that will be activated once UseAbility() is called
		void AbilityEffects();
		void AddModifier(Ability const& ability);
		// Remove 1 turn attribute from each ability modifier
		void UpdateModifiers();
		void CheckModifiers();
		// Change the type of ability this controller is current using
		void ChangeAbility(int abilityId);
		// Use the currently select ability if it has unlimited or any charges remaining
		// return true and the selected ability can be use, otherwise false
		bool UseAbility();
		// Add ability charges from the based on selection
		void AddAbility(int abilityId, int chargesToAdd = 1);
		void UpdateInventory(Ability& ability, bool toAddToVector);
		// Find ability in the AllAbilties vector
		Ability* GetAbility(int abilityId);
		Ability* GetAbility(std::string abilityName);
		Ability* GetAbilityModifiers(std::string abilityName);
		// return the projectile Id to change the type of projectile the CANNON is currently using
		// if the ability type is an offensive which affects the projectile
		int GetProjectileId();

		// Reset to default ability if current ability if an offensive and charges remaining is 0 to 
		// ensure that an ability with 0 charges cannot be used again
		void ResetForNewTurn() {
			ChangeAbility(0);
			isAbilityTriggered = false;
		}

		// True to toggle through all abilities or 
		// false to toggle through of all the inventory abilities this controller has
		void ToggleAbility(bool isAllAbilities, int addValue);

		// AI Member functions
		void ResetAIAbilitySelection() { hasEnemySelectedAbility = false; }
		void AIAbilitySelection();

		const Ability AbilityRandomisator(std::vector<Ability> const& abilities);

		void AbilitySelectionRandomisator();
		void AbilitySelector();
		void OnClickAbilityCard(Ability const& ability);

		void InitializeAllAbilityUI();
		void AbilitySelectorUI();
		void UpdateAbilityUI();

	private:
		void PrintInventory();
		void PrintAllAbilities();
		void PrintInventoryCount();
		void PrintAbilityModifiers();

	private:
		std::vector<Ability> offensiveAbilities; //vector for offensive abils
		std::vector<Ability> statsAbilities; //vector for stats abils
		std::vector<Ability> defensiveAbilities; //vector for defensive abils

		std::vector<Ability> allAbilities; // vector containing all the different type of abilities
		std::vector<Ability> allProjectilesAbilties; // vector containing all the ability that uses it own projectile mode
		std::vector<Ability> abilityModifiers;// vector containing all the active abilites that last for more than 1 turn (HP boost, attack boost, etc)

		std::string controllerName; // use for debugging
		Ability* currentAbility = nullptr;

		std::vector<Ability*> inventory; // vector containing all the type of abilities this controller has in its inventory
		std::vector<int> inventoryTypeCount; // To keep track of how many of each ability type there is in the inventory

		size_t totalAbilityCount = 0;
		int inventoryCount = 0; // total number of abilites the has stored in the inventory
		int currentInventoryId = 0;
		// ids of all the ability that store in the inventory 
		// for example inventory has 5 max space [0][0][1][2][3] < numbers represents the abilityId 
		// has 2 charges of abilityId in inventory while the rest 1 charges only
		std::vector<int> inventoryAbilityIds; 
		bool isSelectingAbility = false;
		bool isAbilityTriggered = false;
		bool isPlayerControlled = true;
		bool hasEnemySelectedAbility = false; // has enemy selected their ability

		std::vector<Ability> abilitiesCards; // vector containing the 3 abilities to be displayed in the ability selector for the player to select from
	};

	extern AbilityController playerAbilityController;
	extern AbilityController enemyAbilityController;
}

#endif // !ABILITIESMANAGER_HPP
