#include "pch.hpp"

#include "CannonManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"

namespace CannonManager {
	Projectile::Projectile() : timer{ 0.0f }, pos{ 0.0f, 0.0f }, velocity{ 0.0f,0.0f } {} // Default intializer
	Projectile::Projectile(AEVec2 _pos, AEVec2 _velocity) : timer{ 0.0f }, pos(_pos), velocity(_velocity) {}

	void Projectile::Render() {
		AEGfxTexture* tex = AssetsManager::GetTexture("Assets/square.png");
		AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, 0, projectileSize, projectileSize);
		//std::cout << cannonAngle << std::endl;
		GraphicsManager::DrawGameObject(tex, mat, Color{ 0.5f, 0.5f, 0, 1 });
	}

	void Projectile::MoveProjectile(float dt) {
		velocity.y -= gravity * dt;

		pos.x += velocity.x * dt;
		pos.y += velocity.y * dt;
	}

	Cannon::Cannon() : pos{ 0.0f,0.0f }, cannonAngle{ 0.0f }, ammo{ NULL }, ammoLimit{ 0 }, shotPower{ power } {}
	Cannon::Cannon(AEVec2 _pos, int _projectileAmount) : pos(_pos), ammoLimit(_projectileAmount), cannonAngle{ 0.0f }, shotPower{ power } {
		ammo.clear();
		ammo.resize(ammoLimit);
		activeAmmo.clear();
		ammoCount = 0;
	}

	void Cannon::Update() {
		// Calculate cannon angle
		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		float worldMouseX = (float)mouseX - (Config::SCREEN_WIDTH / 2.0f);
		float worldMouseY = (Config::SCREEN_HEIGHT / 2.0f) - (float)mouseY;

		// Calculate angle toward mouse
		cannonAngle = atan2(worldMouseY - pos.y, worldMouseX - pos.x);

		// Add limits so it can only turn facing upwards or downwards limits
		cannonAngle = cannonAngle < -1.5 ? -1.5f : cannonAngle;
		cannonAngle = cannonAngle > 1.5 ? 1.5f : cannonAngle;

		Render();

		float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());
		ChargeShot(dt);

		ProjectileUpdate(dt);
	}

	void Cannon::ChargeShot(float dt) {
		if (AEInputCheckCurr(VK_LBUTTON)) {
			float chargeRateWithDT = chargeRate * dt;
			power += isPowerIncrease ? chargeRateWithDT : -chargeRateWithDT;
			if (power > maxPower) {
				power = maxPower;
				isPowerIncrease = false;
			}
			if (power < minPower) {
				power = minPower;
				isPowerIncrease = true;
			}

			UIChargeShotBar();
			//std::cout << power << std::endl;
		}

		// Shoot when the button is RELEASED
		if (AEInputCheckReleased(VK_LBUTTON)) {
			shotPower = power;
			ShootProjectile(); // Pass the built-up power
			power = minPower; // Reset for next shot
		}
	}

	void Cannon::Render() {
		AEGfxTexture* tex = AssetsManager::GetTexture("Assets/square.png");
		AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, cannonAngle, cannonSize.x, cannonSize.y);
		//std::cout << cannonAngle << std::endl;
		GraphicsManager::DrawGameObject(tex, mat, Color{ 1, 0, 0, 1 });
	}

	void Cannon::ShootProjectile() {
		// Assign values to an projectile
		ammo[ammoCount].isActive = true;
		ammo[ammoCount].id = ammoCount;

		// Add offset to ensure that the projectile spawn at the barrel instead of the middle of the cannon
		ammo[ammoCount].pos.x = pos.x + cos(cannonAngle) * cannonSize.x;
		ammo[ammoCount].pos.y = pos.y + sin(cannonAngle) * cannonSize.x;

		// Calculate velocity here once only to ensure that gravity has an effect
		ammo[ammoCount].velocity.x = cos(cannonAngle) * shotPower;
		ammo[ammoCount].velocity.y = sin(cannonAngle) * shotPower;
		ammo[ammoCount].timer = projectileLifespan;

		// Add to the vector lisst
		activeAmmo.push_back(ammo[ammoCount]);

		// Once it reach the ammo limit reset the ammo count
		if (++ammoCount >= ammoLimit) {
			ammoCount = 0;
		}

		std::cout << "Projectile Fired!" << " " << ammoCount << std::endl;
	}

	void Cannon::ProjectileUpdate(float dt) {
		for (Projectile& bullet : activeAmmo) {
			if (bullet.isActive == true) {
				bullet.Render();
				bullet.MoveProjectile(dt);
				bullet.timer -= 1 * dt;

				// Destroy the projectile once it has reach its lifespan or something else
				if (bullet.timer <= 0.0f) {
					DestroyProjectile(bullet);
				}
				//std::cout << bullet.timer << std::endl;
			}
		}

		//std::cout << activeProjectile.size() << std::endl;
	}

	void Cannon::DestroyProjectile(Projectile &projectile) {
		projectile.isActive = false;
		// Find the index containing the projectile and erase from the active ammo vector
		for (int i = 0; i < activeAmmo.size(); i++) {
			if (activeAmmo[i].id == projectile.id) {
				activeAmmo.erase(activeAmmo.begin() + i);

				std::cout << "Removing " << i << " " << "Count " << activeAmmo.size() << std::endl;
			}
		}
	}

	void Cannon::UIChargeShotBar() {
		AEGfxTexture* tex = AssetsManager::GetTexture("Assets/square.png");

		float x = pos.x - (cannonSize.x*1.75f), y = pos.y;
		float w = 20, h = 75, gap = 5;

		// Background Border Bar
		AEMtx33 mat = GraphicsManager::CreateTransformMatrix(x, y, 0, w, -h);
		GraphicsManager::DrawUI(tex, mat, Color{ 0, 0, 0, 1 });

		w = w - (gap * 2), h = h - (gap * 2);

		x += gap, y += gap;

		// Lighter background bar
		AEMtx33 mat2 = GraphicsManager::CreateTransformMatrix(x, y, 0, w,-h);
		GraphicsManager::DrawUI(tex, mat2, Color{ 0, 0.2f, 0, 1 });

		float percentage = (power / maxPower);
		float newH = h*percentage;

		// Bar
		AEMtx33 mat3 = GraphicsManager::CreateTransformMatrix(x, y, 0, w, -newH);
		GraphicsManager::DrawUI(tex, mat3, Color{ 0, 0.8f, 0, 1 });
	}
}