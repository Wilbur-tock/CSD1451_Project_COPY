#include "AbilitiesManager.hpp"

namespace AbilitiesManager {
	void SetProjectileAbility(Projectile& projectile) {
		switch (projectile.Mode()) {
		case BOMB: // 3 tile radius AOE Instant damage
			projectile.attribute.DamageRadius(3);
			projectile.attribute.Damage(100);
			break;
		}
	}
}