#include "AbilitiesManager.hpp"
#include "Color.hpp"
#include "UIManager.hpp"

namespace AbilitiesManager {
	AbilityController playerAbilityController;
	AbilityController enemyAbilityController;

	void AbilityController::LoadAllAbilities() {
		int count = 3;
		for (int i = 0; i < count; ++i) {
			Ability ability{ i, abilitiesName[i], AbilityType::OFFENSIVE, 0, true };

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

		// Testing add bomb projectile with 10 charges
		AddAbility(1, 10);
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
}