#ifndef ABILITIESMANAGER_HPP
#define ABILITIESMANAGER_HPP

#include "pch.hpp"

#include "Projectile.hpp"
#include "Ability.hpp"

namespace AbilitiesManager {
	class AbilityController {
	public:
		void ChangeAbility(int id);
		bool UseAbility();
		// Buy or gain ability charges from the shop
		void AddAbility(int id, int chargesToAdd);

		Ability* FindAbility(int id);
		int FindProjectileId();
		void LoadAllAbilities();
	private:
		Ability* currentAbility = nullptr;
		std::vector<Ability> activeAbilities;
		std::vector<Ability> abilities; 
	};

	inline std::string abilitiesName[] = {
		"Default Projectile", "Bomb Projectile", "Fast Projectile",
	};

	void SetProjectileAbility(Projectile& projectile);
	void RenderUI();
}

#endif // !ABILITIESMANAGER_HPP
