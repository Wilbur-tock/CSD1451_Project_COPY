#include "Attributes.hpp"

void Attributes::Heal(float percentage) {
	Health(health + static_cast<int>(maxHealth * percentage));
}

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

void Attributes::DamageShield(float remainingHPPercentage) {
	if (!IsShieldActive()) return;

	remainingHPPercentage = std::clamp(remainingHPPercentage, 0.0f, 1.0f);
	Shield(static_cast<int>(shield * remainingHPPercentage));
}

void Attributes::SetTickDamage(int tickDmg, int ticks) {
	isTickDamageActive = true;
	tickDamage = tickDmg;
	remainingTicks = ticks;
}

void Attributes::SetInvisibility(int ticks) {
	isInvisible = true;
	canBeDamaged = false;
	remainingTicks = ticks;
}

void Attributes::UpdateTicks() {
	if (!HasModifier()) return;

	if (isTickDamageActive) {
		TakeDamage(tickDamage);
		--remainingTicks;
	}

	if (remainingTicks <= 0) {
		isTickDamageActive = false;

		// Reset invisible
		if (isInvisible) canBeDamaged = true;
		isInvisible = false;
	}
}

std::string Attributes::TickDamageInfo() {
	if (!isTickDamageActive) return "";
	return "\n\nTicks Remaining: " + std::to_string(remainingTicks)
		+ "\nTicks Damage: " + std::to_string(tickDamage);
}