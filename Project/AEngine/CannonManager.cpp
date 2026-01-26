#include "pch.hpp"

#include "CannonManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Color.hpp"
#include "CameraManager.hpp"

namespace CannonManager {
	Projectile::Projectile() : timer{ 0.0f }, pos{ 0.0f, 0.0f }, velocity{ 0.0f,0.0f } {}
	Projectile::Projectile(AEVec2 _pos, AEVec2 _velocity) : timer{ 0.0f }, pos(_pos), velocity(_velocity) {}

	void Projectile::Render() {
		AEGfxTexture* tex = AssetsManager::GetTexture("Assets/circle.png");
		AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, 0, projectileSize, projectileSize);
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

		// Different way to calculate world position of the mouse -MAX_MAP_WIDTH/2.f to MAX_MAP_WIDTH
		float worldMouseX = (float)mouseX - (Config::SCREEN_WIDTH / 2.0f);
		float worldMouseY = (Config::SCREEN_HEIGHT / 2.0f) - (float)mouseY;

		// Add camera offset 
		f32 camX = 0, camY = 0;
		AEGfxGetCamPosition(&camX, &camY);
		worldMouseX += camX;
		worldMouseY += camY;

		// Calculate angle toward mouse
		cannonAngle = atan2(worldMouseY - pos.y, worldMouseX - pos.x);

		// Add limits so it can only turn facing upwards or downwards limits
		cannonAngle = cannonAngle < -1.5 ? -1.5f : cannonAngle;
		cannonAngle = cannonAngle > 1.5 ? 1.5f : cannonAngle;

		// Debugger
		float mX = 0, mY = 0;
		Utils::GetWorldMousePos(mX, mY);

		float cX = pos.x, cY = pos.y;
		Utils::GetWorldPos(cX, cY);
		std::string str = "Cannon\nPos X: " + Utils::FormatFloat(pos.x, 2) + " Y: " + Utils::FormatFloat(pos.y, 2)
			+ "\nWorld Pos X: " + Utils::FormatFloat(cX, 2) + " Y: " + Utils::FormatFloat(cY, 2)
			+ "\nAngle: " + Utils::FormatFloat(cannonAngle * (180.0f / 3.14159265359), 2)
			+ "\nMouse\nScreen Pos X: " + Utils::FormatFloat(mouseX, 2) + " Y: " + Utils::FormatFloat(mouseY, 2)
			+ "\nWorld Pos (-" + Utils::FormatFloat(Config::WORLD_MAX_X, 2) + " to " + Utils::FormatFloat(Config::WORLD_MAX_X, 2) + ")"
			+ "\nX: " + Utils::FormatFloat(worldMouseX, 2) + " Y : " + Utils::FormatFloat(worldMouseY, 2)
			+ "\nWorld Pos (0.00" + " to " + Utils::FormatFloat(Config::WORLD_MAX_X * 2, 2) + ")"
			+ "\nX: " + Utils::FormatFloat(mX, 2) + " Y: " + Utils::FormatFloat(mY, 2);

		GraphicsManager::DrawTextUI(str, -1, 0.99f, Color{ 0,0,1 }, 0.8f);

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
		AEGfxTexture* tex = AssetsManager::GetTexture("Assets/test.png");
		AEMtx33 mat = GraphicsManager::CreateTransformMatrix(pos.x, pos.y, cannonAngle, cannonSize.x, cannonSize.y);
		GraphicsManager::DrawGameObject(tex, mat, Color{ 1, 1, 1, 1 });
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

		// Add to the vector list
		activeAmmo.push_back(ammo[ammoCount]);

		// Set camera to follow the newly fired projectile
		CameraController::SetFollowTarget(&activeAmmo.back().pos);
		CameraController::SetState(CameraController::CameraState::FOLLOW);

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
			}
		}
	}

	void Cannon::DestroyProjectile(Projectile& projectile) {
		projectile.isActive = false;
		// Find the index containing the projectile and erase from the active ammo vector
		for (int i = 0; i < activeAmmo.size(); i++) {
			if (activeAmmo[i].id == projectile.id) {
				activeAmmo.erase(activeAmmo.begin() + i);

				// Return camera to input control when all projectiles are destroyed
				if (activeAmmo.empty()) {
					CameraController::SetFollowTarget(nullptr);
					CameraController::SetState(CameraController::CameraState::INPUT);
				}

				std::cout << "Removing " << i << " " << "Count " << activeAmmo.size() << std::endl;
			}
		}
	}

	void Cannon::UIChargeShotBar() {
		AEGfxTexture* tex = AssetsManager::GetTexture("Assets/square.png");

		float x = pos.x - (cannonSize.x * 1.75f), y = pos.y;
		float w = 20, h = 75, gap = 5;

		// Background Border Bar
		AEMtx33 mat = GraphicsManager::CreateTransformMatrix(x, y, 0, w, -h);
		GraphicsManager::DrawUI(tex, mat, Color{ 0, 0, 0, 1 });

		w = w - (gap * 2), h = h - (gap * 2);

		x += gap, y += gap;

		// Lighter background bar
		AEMtx33 mat2 = GraphicsManager::CreateTransformMatrix(x, y, 0, w, -h);
		GraphicsManager::DrawUI(tex, mat2, Color{ 0, 0.2f, 0, 1 });

		float percentage = (power / maxPower);
		float newH = h * percentage;

		// Bar
		AEMtx33 mat3 = GraphicsManager::CreateTransformMatrix(x, y, 0, w, -newH);
		GraphicsManager::DrawUI(tex, mat3, Color{ 0, 0.8f, 0, 1 });
	}
}