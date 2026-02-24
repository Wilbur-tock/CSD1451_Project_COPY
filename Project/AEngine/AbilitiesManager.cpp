#include "AbilitiesManager.hpp"
#include "Color.hpp"
#include "UIManager.hpp"

namespace AbilitiesManager {
	void AbilityController::ChangeAbility(int id) {
		currentAbility = FindAbility(id);

		std::cout << "[AbilitiesManager] Current ability: " + abilitiesName[currentAbility->Id()]
			+ " Charges Remaining: " + std::to_string(currentAbility->Charges()) << std::endl;
	}

	bool AbilityController::UseAbility() {
		if (currentAbility->UnlimitedCharges()) return true;
		if (currentAbility->Charges() <= 0) return false;

		currentAbility->Charges(-1);

		// Update projectile to DEFAULT projectile once this ability charges is all used up
		if (currentAbility->Type() == AbilityType::OFFENSIVE_PROJECTILE && currentAbility->Charges() <= 0) {
			ChangeAbility(0);
		}

		std::cout << "[AbilitiesManager] Ability used: " + abilitiesName[currentAbility->Id()] 
			+ " Charges Remaining: " + std::to_string(currentAbility->Charges()) << std::endl;

		return true;
	}

	void AbilityController::AddAbility(int id, int chargesToAdd) {
		Ability& ability = *FindAbility(id);
		ability.Charges(chargesToAdd);
	}

	Ability* AbilityController::FindAbility(int id) {
		for (Ability& ab : abilities) {
			if (ab.Id() == id) {
				return &ab;
			}
		}

		std::cerr << "[AbilitiesManager] Can't find " + std::to_string(id) + " in the abilities list" << std::endl;
		return nullptr;
	}

	int AbilityController::FindProjectileId() {
		if (currentAbility->Type() != AbilityType::OFFENSIVE_PROJECTILE) return 0;
		return currentAbility->ProjectileId();
	}

	void AbilityController::LoadAllAbilities() {
		int count = 3;
		for (int i = 0; i < count; ++i) {
			Ability ability{ i, abilitiesName[i], AbilityType::OFFENSIVE_PROJECTILE, 0};

			switch (i) {
			case 0:
				ability.UnlimitedCharges(true);
				break;
			case 1:
				ability = { i, abilitiesName[i], AbilityType::OFFENSIVE_PROJECTILE, 1};
				break;
			case 2:
				ability = { i, abilitiesName[i], AbilityType::OFFENSIVE_PROJECTILE, 2 };
				break;
			}

			abilities.push_back(ability);
		}

		AddAbility(1, 10);

		currentAbility = &abilities[0];
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

	void RenderUI() {
		UIManager::Panel panel = { {450,100}, {0.9f,0.9f,0.9f}, Utils::RECT_BOTTOM_CENTER };
		panel.Draw();
		UIManager::Panel subPanel = { {50,50}, {0,1,0} };
		subPanel.SetChildPanelPos(panel.Pos(), { 5,-5 });
		subPanel.Draw();
	}
}