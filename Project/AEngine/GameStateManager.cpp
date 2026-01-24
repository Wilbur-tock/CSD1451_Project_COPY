#include "pch.hpp"

#include "MapManager.hpp"
#include "MapEditor.hpp"
#include "CannonManager.hpp"

namespace GameStateManager {

	FP fpLoad, fpInitalize, fpUpdate, fpDraw, fpUnload, fpExit;
	int current = 0, previous = 0, next = 0;

	// Temporary
	AEGfxTexture* texture;

	AEVec2 v2{ -700,-325 };
	CannonManager::Cannon cannon(v2, 50);

	void Load() {
		GraphicsManager::Load();

		texture = AssetsManager::GetTexture("Assets/square.png");

		MapManager::LoadAllMapData();
	}

	bool mapEditorOn = false;

	f32 x = 0, y = 0;
	void Update() {
		switch (current) {
		case GS_Main_Menu:
			break;
		case GS_Level_1:
			break;
		case GS_Level_2:
			break;
		}

		if (AEInputCheckTriggered(AEVK_P)) {
			mapEditorOn = !mapEditorOn;
		}

		if (mapEditorOn) {
			MapEditor::MapEditor();
		}
		else {
			MapManager::RenderMap(texture, MapManager::mapLayout);

			cannon.Update();
		}

		// Temporary
		float spd = 1500.0f * static_cast<float>(AEFrameRateControllerGetFrameTime());
		if (AEInputCheckCurr(AEVK_A)) x -= spd;
		if (AEInputCheckCurr(AEVK_D)) x += spd;

		float minX = Config::WORLD_MIN_X + (Config::SCREEN_WIDTH / 2.0f);
		float maxX = Config::WORLD_MAX_X - (Config::SCREEN_WIDTH / 2.0f);

		if (x < minX) x = minX;
		if (x > maxX) x = maxX;

		AEGfxSetCamPosition(floorf(x), floorf(y));
	}

	void Exit() {

	}
}
