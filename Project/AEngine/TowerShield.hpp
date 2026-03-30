/*
Authors: Dayrius
Description: Stores data and functions about the Tower Shield mechanic

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef TOWERSHIELD_HPP
#define TOWERSHIELD_HPP

#include "Attributes.hpp"
#include "GameObject.hpp"
#include "AbilitiesManager.hpp"

class TowerShield : public GameObject {
public:
	Attributes attribute;

	TowerShield() = default;
	TowerShield(AEVec2 _pos, AbilitiesManager::AbilityController* controller);

	Transform const& GetTransform() const { return transform; }
	bool IsShieldOn() const { return isShieldOn; }
	void SetShieldOn(int shield, int turnLifespan);

	void Draw();
	void TakeDamage(int damage, bool isShieldBuster = false);
	void UpdateLifespan();

	void DebugInfo(std::string& str);
private:
	bool isShieldOn = false;
	int lifespan = 0; // number of turns the shield stays alive before disappearing
	AbilitiesManager::AbilityController* abilityController = nullptr;
};

#endif // !TOWERSHIELD_HPP
