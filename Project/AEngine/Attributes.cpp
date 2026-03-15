#include "Attributes.hpp"

void Attributes::TakeDamage(int value) {
	if (!canBeDamaged) return;

	if (IsShieldActive()) {
		Shield(--shield);
	}
	else {
		int damageVal = static_cast<int>(value * (1.0f - damageReduction));
		Health(health - damageVal);
	}
}

void Attributes::DamageShield(float percentage) {
	if (!IsShieldActive()) return;

	percentage = std::clamp(percentage, 0.0f, 1.0f);
	Shield(static_cast<int>(shield * percentage));
}