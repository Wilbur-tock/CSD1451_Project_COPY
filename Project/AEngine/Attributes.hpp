#ifndef ATTRIBUTES_HPP
#define ATTRIBUTES_HPP

#include "pch.hpp"

class Attributes {
private:
	int const minValue = 0, maxValue = 100;
	int health, shield;
	float damageReduction, critReduction;
public:
	Attributes();

	int GetHealth() const;
	void SetHealth(int newValue);

	void Damage(int damage);
};

#endif // !ATTRIBUTES_HPP
