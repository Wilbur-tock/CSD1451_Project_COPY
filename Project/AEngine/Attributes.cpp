#include "Attributes.hpp"

void Attributes::TakeDamage(int damageValue) {
	if (!canBeDamaged) return;

	if (shield <= 0) {
		int damageVal = static_cast<int>(damageValue * (1.0f - damageReduction));
		Health(health - damageVal);
	}
	else {
		Shield(--shield);
	}
}