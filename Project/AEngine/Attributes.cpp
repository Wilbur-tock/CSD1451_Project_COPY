#include "Attributes.hpp"

int  Attributes::Health() const { return health; }
void Attributes::Health(int value) { health = std::clamp(value, minValue, maxHealth); }

int	 Attributes::MaxHealth() const { return maxHealth; }
void Attributes::MaxHealth(int value) { maxHealth = value; }

int  Attributes::Damage() const { return damage; }
void Attributes::Damage(int value) { damage = value; }

void Attributes::TakeDamage(int damageValue) {
	if (!canBeDamaged) return;

	if (shield <= 0) {
		Health(health - damageValue);
	}
	else {

	}
}