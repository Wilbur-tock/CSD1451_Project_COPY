#include "pch.hpp"
#include "CannonManager.hpp"

namespace CannonManager {
	Projectile::Projectile() : timer{ 0.0f }, pos{ 0.0f, 0.0f }, velocity{ 0.0f,0.0f } {} // Default intializer
	Projectile::Projectile(AEVec2 _pos, AEVec2 _velocity) : timer{ 0.0f }, pos(_pos), velocity(_velocity) {}

	void Projectile::Render() {
		AEGfxTexture* tex = AssetsManager::GetTexture("Assets/square.png");
		AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, 0, projectileSize, projectileSize);
		//std::cout << cannonAngle << std::endl;
		GraphicsManager::DrawGameObject(tex, mat, 0.5f, 0.5f, 0, 1);
	}

	void Projectile::MoveProjectile(float speed, float dt) {
		velocity.y -= gravity * dt;

		pos.x += velocity.x * dt;
		pos.y += velocity.y * dt;
	}

	void Projectile::DestroyProjectile() {
		isActive = false;
	}

	Cannon::Cannon() : pos{ 0.0f,0.0f }, cannonAngle{ 0.0f }, bullets{ NULL }, projectileAmount{ 0 }, projectileSpeed{ power } {}
	Cannon::Cannon(AEVec2 _pos, int _projectileAmount) : pos(_pos), projectileAmount(_projectileAmount), cannonAngle{ 0.0f }, projectileSpeed{ power } {
		bullets.resize(projectileAmount);
	}

	void Cannon::Update() {
		// Calculate cannon angle
		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		float worldMouseX = (float)mouseX - (Config::screenWidth / 2.0f);
		float worldMouseY = (Config::screenHeight / 2.0f) - (float)mouseY;

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

			UI();
			std::cout << power << std::endl;
		}

		// Shoot when the button is RELEASED
		if (AEInputCheckReleased(VK_LBUTTON)) {
			projectileSpeed = power;
			ShootProjectile(dt); // Pass the built-up power
			power = minPower; // Reset for next shot
		}
	}

	void Cannon::Render() {
		AEGfxTexture* tex = AssetsManager::GetTexture("Assets/square.png");
		AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, cannonAngle, cannonSize.x, cannonSize.y);
		//std::cout << cannonAngle << std::endl;
		GraphicsManager::DrawGameObject(tex, mat, 1, 0, 0, 1);
	}

	void Cannon::ShootProjectile(float dt) {
		// Assign values to an projectile
		bullets[projectileUsed].isActive = true;

		// Add offset to ensure that the projectile spawn at the barrel instead of the middle of the cannon
		bullets[projectileUsed].pos.x = pos.x + cos(cannonAngle) * cannonSize.x;
		bullets[projectileUsed].pos.y = pos.y + sin(cannonAngle) * cannonSize.x;

		// Calculate velocity here once only to ensure that gravity has an effect
		bullets[projectileUsed].velocity.x = cos(cannonAngle) * projectileSpeed;
		bullets[projectileUsed].velocity.y = sin(cannonAngle) * projectileSpeed;
		bullets[projectileUsed].timer = projectileLifespan;

		// Add to the vector
		activeProjectile.push_back(bullets[projectileUsed]);
		projectileUsed++;

		std::cout << "Projectile Fired!";
	}

	void Cannon::ProjectileUpdate(float dt) {
		for (Projectile& bullet : activeProjectile) {
			if (bullet.isActive == true) {
				bullet.Render();
				bullet.MoveProjectile(projectileSpeed, dt);
				bullet.timer -= 1 * dt;

				// Destroy the projectile once it has reach its lifespan or something else
				if (bullet.timer <= 0.0f) bullet.DestroyProjectile();
				std::cout << bullet.timer << std::endl;
			}
		}
	}

	void Cannon::UI() {
		AEGfxTexture* tex = AssetsManager::GetTexture("Assets/square.png");

		float x = pos.x - (cannonSize.x*1.75f), y = pos.y;
		float w = 20, h = 75, gap = 5;

		//Background Bar
		AEMtx33 mat = GraphicsManager::CreateTransformMatrix(x, y, 0, w, -h);
		GraphicsManager::DrawUI(tex, mat, 0, 0, 0, 1);

		w = w - (gap * 2), h = h - (gap * 2);

		x += gap, y += gap;

		AEMtx33 mat2 = GraphicsManager::CreateTransformMatrix(x, y, 0, w,-h);
		GraphicsManager::DrawUI(tex, mat2, 0, 0.2, 0, 1);

		float percentage = (power / maxPower);
		float newH = h*percentage;

		AEMtx33 mat3 = GraphicsManager::CreateTransformMatrix(x, y, 0, w, -newH);
		GraphicsManager::DrawUI(tex, mat3, 0, 0.8, 0, 1);
	}
}