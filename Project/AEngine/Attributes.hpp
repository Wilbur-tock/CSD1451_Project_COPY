#ifndef ATTRIBUTES_HPP
#define ATTRIBUTES_HPP

#include "pch.hpp"

// Attributes used for projectile, tiles, enemy and player characters
class Attributes {
public:
	Attributes() = default;

	int  Health() const { return health; }
	void Health(int value) { 
		health = std::clamp(value, 0, maxHealth); 
		isActive = health > 0;
	}

	// Shield take 1 damage per attack regardless of damage value
	int  Shield() const { return shield;  }
	bool IsShieldActive() const { return shield > 0; }

	int	 MaxShield() const { return maxShield; }
	void SetShield(int value) { maxShield = value; shield = value; }

	int	 MaxHealth() const { return maxHealth; }
	void MaxHealth(int value) { maxHealth = value; }

	int  Damage() const { return damage; }
	void Damage(int value) { damage = value; }

	int  DamageRadius() const { return damageRadius; }
	void DamageRadius(int value) { damageRadius = value; }

	int	 DamageMultiplier() const { return damageMultiplier; }
	void DamageMultiplier(int value) { damageMultiplier = std::clamp(value, 1, 10); }

	float DamageReduction() const { return damageReduction; }
	void  DamageReduction(float percentage) { damageReduction = std::clamp(percentage, 0.0f, 1.0f); }

	// return true if attribute health is more than 0 otherwise false
	bool IsActive() const { return isActive; }

	void TakeDamage(int damage);
private:
	int minValue = 0, maxValue = 100;
	bool canBeDamaged = true;
	bool isActive = true; // return true if health > 0 otherwise false

	int damage = 10;
	int damageRadius = 1;

	int health = maxValue;
	int maxHealth = maxValue;

	int shield = 0;
	int maxShield = 0;

	int damageMultiplier = 1; // multiply how much damage it can take, 1 - normal damage, 5 - times 5 of damage
	// Percentages = 0.00 - 1.00
	float damageReduction = 0, critReduction = 0;
};

#endif // !ATTRIBUTES_HPP
