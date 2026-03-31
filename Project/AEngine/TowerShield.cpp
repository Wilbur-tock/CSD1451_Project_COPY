/*
Authors: Dayrius
Description: Stores data and functions about the Tower Shield mechanic

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

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
	isActive = true;
	attribute.SetShield(shield);
	lifespan = _turnLifespan;
}

void TowerShield::Draw() {
	if (!isActive) return;
	GraphicsManager::DrawGameObject(*this);

	if (Utils::isDebuggerOn) {
		DrawCollisionLine();
	}
}

void TowerShield::TakeDamage(int damage, bool isShieldBuster) {
	if (isShieldBuster) {
		attribute.DamageShield(0.0f);
	}
	else {
		attribute.TakeDamage(damage);
	}

	// reduce shield attribute on the abilityModifiers vector also 
	// so that it can remove from the abilityModifiers vector once the shield attribute reaches 0
	abilityController->GetAbilityModifiers("Tower Shield")->SetShield(attribute.Shield());
	abilityController->CheckModifiers();

	isActive = attribute.IsShieldActive();
}

void TowerShield::UpdateLifespan() {
	if (!isActive) return;
	--lifespan;

	if (lifespan <= 0) {
		isActive = false;
	}
}

void TowerShield::DebugInfo(std::string& str) {
	if (!isActive) return;
	str += "\n\nPlayer Shield:\nHP: " + std::to_string(attribute.Shield())
		+ "\nLifespan: " + std::to_string(lifespan);
}