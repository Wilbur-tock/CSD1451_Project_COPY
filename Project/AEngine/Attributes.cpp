#include "Attributes.hpp"

void Attributes::Heal(float percentage) {
	Health(static_cast<int>(maxHealth * percentage));
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

void Attributes::DamageShield(float percentage) {
	if (!IsShieldActive()) return;

	percentage = std::clamp(percentage, 0.0f, 1.0f);
	Shield(static_cast<int>(shield * percentage));
}

void Attributes::SetTickDamage(int tickDmg, int ticks) {
	isTickDamageActive = true;
	tickDamage = tickDmg;
	remainingTicks = ticks;
	std::cout << "Poison! " << ticks <<'\n';
}

void Attributes::SetInvisibility(int ticks) {
	isInvisible = true;
	canBeDamaged = false;
	remainingTicks = ticks;
}

void Attributes::UpdateTicks() {
	if (!HasModifier()) return;
	std::cout << std::boolalpha << HasModifier() << '\n';

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