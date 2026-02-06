#include "Level1.hpp"

#include "MapManager.hpp"
#include "CannonManager.hpp"
#include "Level.hpp"
#include "CameraManager.hpp"
#include "TextObject.hpp"
#include "ParticleSystem.hpp"

namespace Level1 {
	Level levelData;
	
	// Particle emitter for demonstration
	ParticleSystem::ParticleEmitter* particleEmitter = nullptr;

	void Load() {
		levelData.Load("Assets/Map/level1.txt");
	}

	void Init() {
		AEGfxSetBackgroundColor(0.92f, 0.92f, 0.95f);
		CameraManager::camera.SetPosition(levelData.cannon.pos);
		CameraManager::camera.SetState(CameraManager::CameraController::INPUT);
		levelData.Init();
		
		// Initialize particle emitter at the cannon's position
		AEVec2 emitterPos;
		AEVec2Set(&emitterPos, levelData.cannon.pos.x, levelData.cannon.pos.y + 100.0f);
		particleEmitter = new ParticleSystem::ParticleEmitter(emitterPos, 200);
		
		// Configure particle properties
		particleEmitter->SetEmissionRate(30.0f);  // 30 particles per second
		particleEmitter->SetLifetimeRange(1.0f, 3.0f);
		particleEmitter->SetSpeedRange(80.0f, 150.0f);
		particleEmitter->SetSizeRange(8.0f, 15.0f);
		particleEmitter->SetColor(Color(1.0f, 0.7f, 0.2f, 1.0f));  // Orange particles
		particleEmitter->SetFadeOut(true);
	}

	void Update() {
		MapManager::RenderMap(levelData.levelMap);
		levelData.cannon.Update();
		
		// Update particle system
		if (particleEmitter) {
			particleEmitter->Update(GameStateManager::dt);
		}

		if (Utils::isDebuggerOn) {
			f32 x, y;
			AEGfxGetCamPosition(&x, &y);

			s32 mouseX, mouseY;
			AEInputGetCursorPosition(&mouseX, &mouseY);
			float mX = 0, mY = 0;
			Utils::GetWorldMousePos(mX, mY);

			int gridX = 0, gridY = 0;
			Utils::GetMouseGridIndex(gridX, gridY);

			MapManager::Tile& tile = (*MapManager::currentMap)[gridY][gridX];

			levelData.debugText.textContent = "Grid:\nIndex X: " + std::to_string(gridX) + " Y: " + std::to_string(gridY)
				+ "\nTile: " + MapManager::TileTypeToString(tile.type) + " " + Utils::FormatFloat(tile.GetHealth(), 2) +
				+"\nPos X: " + Utils::FormatFloat(tile.pos.x, 2) + " Y: " + Utils::FormatFloat(tile.pos.y, 2)
				+ "\nCamera " + std::to_string(CameraManager::camera.GetState()) +
				+ "\nPos X : " + Utils::FormatFloat(x, 2) + " Y : " + Utils::FormatFloat(y, 2)
				+ "\nMouse\nScreen Pos X: " + Utils::FormatFloat(mouseX, 2) + " Y: " + Utils::FormatFloat(mouseY, 2)
				+ "\nWorld Pos X: " + Utils::FormatFloat(mX, 2) + " Y: " + Utils::FormatFloat(mY, 2)
				+ "\n\nParticles: " + std::to_string(particleEmitter ? particleEmitter->GetActiveParticleCount() : 0);
			GraphicsManager::DrawTextUI(levelData.debugText);
		}
	}

	void Draw() {
		// Render particle system
		if (particleEmitter) {
			particleEmitter->Render();
		}
	}

	void Free() {
		// Clean up particle emitter
		if (particleEmitter) {
			delete particleEmitter;
			particleEmitter = nullptr;
		}
	}

	void Unload() {

	}
}