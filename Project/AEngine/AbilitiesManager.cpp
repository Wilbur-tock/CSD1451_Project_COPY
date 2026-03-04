#include "AbilitiesManager.hpp"

#include "Utils.hpp"
#include "Color.hpp"
#include "UIManager.hpp"
#include "Card.hpp"

namespace AbilitiesManager {
	AbilityController playerAbilityController;
	AbilityController enemyAbilityController;

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