#include "Attributes.hpp"

Attributes::Attributes() : health{ maxValue } {}

int Attributes::GetHealth() const { return health; }
void Attributes::SetHealth(int newValue) { health = std::clamp(newValue, minValue, maxValue); }

void Attributes::Damage(int damage) {
	SetHealth(health - damage);
}