#include "Attributes.hpp"

void Attributes::TakeDamage(int damageValue) {
	if (!canBeDamaged) return;

	if (shield <= 0) {
		Health(health - damageValue);

		if (health <= 0) { isActive = false; }
	}
	else {

	}
}