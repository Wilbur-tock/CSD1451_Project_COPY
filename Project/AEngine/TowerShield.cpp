#include "TowerShield.hpp"

#include "AssetsManager.hpp"
#include "GraphicsManager.hpp"
#include "Utils.hpp"

float radius = Config::TILE_SIZE * 20.0f;

TowerShield::TowerShield(AEVec2 _pos, AbilitiesManager::AbilityController* controller)
	: GameObject{ _pos, {radius,radius}, AssetsManager::circleTexture, TileIndex{0,0}, 
	Color{ShieldColor().Red(), ShieldColor().Green(), ShieldColor().Blue(), 0.25f} }, abilityController{controller} {
	attribute.Health(0);
	collider.type = CollisionType::COLLIDER_CIRCLE;
}

void TowerShield::SetShieldOn(int shield, int _turnLifespan) {
	isShieldOn = true;
	attribute.SetShield(shield);
	lifespan = _turnLifespan;
}

void TowerShield::Draw() {
	if (!isShieldOn) return;
	GraphicsManager::DrawGameObject(*this);

	if (Utils::isDebuggerOn) {
		DrawCollisionLine();
	}
}

void TowerShield::TakeDamage(int damage, bool isShieldBuster, float shieldDmgPercentage) {
	if (isShieldBuster) {
		attribute.DamageShield(shieldDmgPercentage);
	}
	else {
		attribute.TakeDamage(damage);
	}

	// reduce shield attribute on the abilityModifiers vector also 
	// so that it can remove from the abilityModifiers vector once the shield attribute reaches 0
	abilityController->GetAbilityModifiers("Tower Shield")->SetShield(attribute.Shield());
	abilityController->CheckModifiers();

	isShieldOn = attribute.IsShieldActive();
}

void TowerShield::UpdateLifespan() {
	if (!isShieldOn) return;
	--lifespan;

	if (lifespan <= 0) {
		isShieldOn = false;
	}
}

void TowerShield::DebugInfo(std::string& str) {
	str += "\n\nPlayer Shield:\nHP: " + std::to_string(attribute.Shield())
		+ "\nLifespan: " + std::to_string(lifespan);
}