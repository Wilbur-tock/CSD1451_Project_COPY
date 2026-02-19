#include "AbilitiesManager.hpp"
#include "Color.hpp"

#include "UIManager.hpp"

namespace AbilitiesManager {
	void SetProjectileAbility(Projectile& projectile) {
		switch (projectile.Mode()) {
		case BOMB: // 3 tile radius AOE Instant damage
			projectile.attribute.DamageRadius(3);
			projectile.attribute.Damage(10);
			break;
		}
	}

	void RenderUI() {
		UIManager::Panel panel = { {450,100}, {0.9f,0.9f,0.9f}, Utils::RECT_BOTTOM_CENTER };
		panel.Draw();
		UIManager::Panel subPanel = { {50,50}, {0,1,0} };
		subPanel.SetChildPanelPos(panel.Pos(), { 5,-5 });
		subPanel.Draw();
	}
}