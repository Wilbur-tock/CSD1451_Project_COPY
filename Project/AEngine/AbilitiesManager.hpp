#ifndef ABILITIESMANAGER_HPP
#define ABILITIESMANAGER_HPP

#include "pch.hpp"

#include "Projectile.hpp"
#include "Ability.hpp"

namespace AbilitiesManager {
	class AbilityController {
	public:
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

	inline std::string abilitiesName[] = {
		"Default Projectile", "Bomb Projectile", "Fast Projectile",
	};

	extern AbilityController playerAbilityController;
	extern AbilityController enemyAbilityController;

	// Set abilities effect to each of the projectiles based on its mode
	void SetProjectileAbility(Projectile& projectile);
}

#endif // !ABILITIESMANAGER_HPP
