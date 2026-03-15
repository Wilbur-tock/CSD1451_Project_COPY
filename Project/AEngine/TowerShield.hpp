#ifndef TOWERSHIELD_HPP
#define TOWERSHIELD_HPP

#include "Attributes.hpp"
#include "GameObject.hpp"

class TowerShield : public GameObject {
public:
	Attributes attribute;

	TowerShield() = default;
	TowerShield(AEVec2 _pos);

	Transform const& GetTransform() const { return transform; }
	bool IsShieldOn() const { return isShieldOn; }
	void SetShieldOn(int shield, int turnLifespan);

	void Draw();
	void TakeDamage(int damage, bool isShieldBuster = false, float shieldDmgPercentage = 0.0f);
	void UpdateLifespan();

	void DebugInfo(std::string& str);
private:
	bool isShieldOn = false;
	int lifespan = 0; // number of turns the shield stays alive before disappearing
};

#endif // !TOWERSHIELD_HPP
