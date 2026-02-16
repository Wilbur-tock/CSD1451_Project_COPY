#ifndef ATTRIBUTES_HPP
#define ATTRIBUTES_HPP

#include "pch.hpp"

// Attributes used for projectile, tiles, enemy and player characters
class Attributes {
private:
	int minValue = 0, maxValue = 100, maxHealth = maxValue;

	int damage = 10, damageRadius = 1;
	bool canBeDamaged = true;

	int health = maxValue, shield = 0;
	float damageReduction = 0, critReduction = 0;
public:
	Attributes() = default;

	int  Health() const;
	void Health(int value);

	int  MaxHealth() const;
	void MaxHealth(int value);

	int  Damage() const;
	void Damage(int value);

	int  DamageRadius() const;
	void DamageRadius(int value);

	void TakeDamage(int damage);
};

#endif // !ATTRIBUTES_HPP
