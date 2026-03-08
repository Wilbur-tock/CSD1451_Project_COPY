#ifndef ABILITIESMANAGER_HPP
#define ABILITIESMANAGER_HPP

#include "pch.hpp"
#include "Projectile.hpp"
#include "Ability.hpp"

namespace AbilitiesManager {
	class AbilityController {
	public:
		// Load all different type of abilites to be called at the start of the game
		void LoadAllAbilities(bool isAi); //Loads all player abils from Abilities.json file
		//void LoadAllAIAbilities(); //Loads all AI abils from AiAbilities.json file
		void LoadStartingAbilities();
		void PrintAllAbilities();

		std::vector<Ability>& GetOffensiveAbilities() { return offensiveAbilities; }
		std::vector<Ability>& GetStatsAbilities() { return statsAbilities; }
		std::vector<Ability>& GetDefensiveAbilities() { return defensiveAbilities; }

		std::vector<Ability>& GetAIOffensiveAbilities() { return AIoffensiveAbilities; }
		std::vector<Ability>& GetAIStatsAbilities() { return statsAbilities; }
		std::vector<Ability>& GetAIDefensiveAbilities() { return defensiveAbilities; }

		void ChangeProjectile(s32 _projectileID);
		bool UseProjectile();
		bool AIUseProjectile();
		Ability* GetCurrentProjectile() { return ProjectileSelected; }
		Ability* GetCurrentAIProjectile() { return AIProjectileSelected; }

		Ability CurrentAbility() const { return *currentAbility; }
		int InventoryCount() const{ return static_cast<int>(inventory.size()); }
		AbilityType OffensiveOrDefensive();

		// Change the type of ability this controller is current using
		void ChangeAbility(int abilityId);
		// Use the currently select ability if it has unlimited or any charges remaining
		// return true and the selected ability can be use, otherwise false
		bool UseAbility();
		// Add ability charges from the based on selection
		void AddAbility(int abilityId, int chargesToAdd = 1);
		void UpdateInventory(Ability& ability, bool toAddToVector);
		Ability* FindAbility(int abilityId);
		// return the projectile Id to change the type of projectile the CANNON is currently using
		// if the ability type is an offensive which affects the projectile
		int FindProjectileId();

		// True to toggle through all abilities or false to toggle through of all the active abilities this controller has in its inventory
		void ToggleAbility(bool isAllAbilities);

		// AI Member functions
		void ResetAIAbilitySelection();
		void AIAbilitySelection();

		const Ability AbilityRandomisator(const std::vector<Ability>& abilities);
		void AbilitySelectionRandomisator();
		void AbilitySelector(bool& isOn);
		void onClickAbilityCard(Ability ability, bool* isOn);

	private:

		Ability* ProjectileSelected = nullptr;// Player selected projectile

		Ability* AIProjectileSelected = nullptr;// AI selected projectile

		std::vector<Ability> offensiveAbilities; //vector for offensive abils
		std::vector<Ability> statsAbilities; //vector for stats abils
		std::vector<Ability> defensiveAbilities; //vector for defensive abils
		
		std::vector<Ability> AIoffensiveAbilities; //vector for AI offensive abils
		std::vector<Ability> AIstatsAbilities; //vector for AI stats abils
		std::vector<Ability> AIdefensiveAbilities; //vector for AI defensive abils

		Ability* currentAbility = nullptr;

		std::vector<Ability*> inventory; // vector containing all the type of abilities this controller has in its inventory
		std::vector<Ability> allAbilities; // vector containing all the different type of abilities

		bool isAIController = false;
		bool hasEnemySelectedAbility = false; // has enemy selected their ability

		std::vector<Ability> abilitiesCards; // vector containing the 3 abilities to be displayed in the ability selector for the player to select from
	};

	extern AbilityController playerAbilityController;
	extern AbilityController enemyAbilityController;
}

#endif // !ABILITIESMANAGER_HPP


		/*
		// Load all different type of abilites to be called at the start of the game
		void LoadAllAbilities();

		// Change the type of ability this controller is current using
		void ChangeAbility(int abilityId);
		// Use the currently select ability if it has unlimited or any charges remaining
		// return true and the selected ability can be use, otherwise false
		bool UseAbility();
		// Add ability charges from the based on selection
		void AddAbility(int abilityId, int chargesToAdd);
		void UpdateActiveAbilites(Ability& ability, bool toAddToVector);
		Ability* FindAbility(int abilityId);
		// return the projectile Id to change the type of projectile the CANNON is currently using
		// if the ability type is an offensive which affects the projectile
		int FindProjectileId();

		// AI Member functions
		void ResetAbilitySelection();
		void AIAbilitySelection();

		void AbilitySelector(bool& isOn);
		void onClickAbilityCard(int abilityID, bool *isOn);

	private:
		Ability* currentAbility = nullptr;
		std::vector<Ability*> activeAbilities; // vector containing all the type of abilities this controller has in its inventory
		std::vector<Ability> abilities; // vector containing all the different type of abilities

		bool hasEnemySelectedAbility = false; // has enemy selected their ability

	};
	/*
	inline std::string abilitiesName[] = {
		"Default Projectile", "Bomb Projectile", "Fast Projectile",
	};

	extern AbilityController playerAbilityController;
	extern AbilityController enemyAbilityController;

	// Set abilities effect to each of the projectiles based on its mode
	void SetProjectileAbility(Projectile& projectile);
	*/
