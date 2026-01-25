#include "pch.hpp"

#include "MapManager.hpp"
#include "MapEditor.hpp"
#include "CannonManager.hpp"
#include "Level1.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"

namespace GameStateManager {
	FP fpLoad, fpInitalize, fpUpdate, fpDraw, fpFree, fpUnload;
	int current = 0, previous = 0, next = 0;

	// Temporary
	AEGfxTexture* texture;

	AEVec2 v2{ -700,-325 };
	CannonManager::Cannon cannon(v2, 50);

	void Load() {
		GraphicsManager::Load();

		current = 0;
		previous = next = GS_Level_1;

		texture = AssetsManager::GetTexture("Assets/square.png");

		Level1::Load();
	}

	bool mapEditorOn = true;

	f32 x = 0, y = 0; //temp

	void Update() {
		// Update Game state
		switch (current) {
		case GS_Main_Menu:
			break;
		case GS_Map_Editor:
			fpLoad = nullptr;
			fpInitalize = nullptr;
			fpUpdate = MapEditor::MapEditor;
			fpDraw = nullptr;
			fpFree = nullptr;
			fpUnload = nullptr;
			break;
		case GS_Level_1:
			fpLoad = Level1::Load;
			fpInitalize = Level1::Initialize;
			fpUpdate = Level1::Update;
			fpDraw = nullptr;
			fpFree = nullptr;
			fpUnload = nullptr;
			break;
		case GS_Level_2:
			break;
		}

		// Update to new state
		if (next != current) {
			previous = current;
			current = next;

			if (fpInitalize != nullptr) {
				fpInitalize();
			}
		}

		if (fpUpdate != nullptr) {
			fpUpdate();
		}

		if (AEInputCheckTriggered(AEVK_Q)) {
			mapEditorOn = !mapEditorOn;
		}

		if (mapEditorOn) {
			next = GS_Map_Editor;
		}
		else {
			next = GS_Level_1;
		}

		// Temporary
		float spd = 1500.0f * static_cast<float>(AEFrameRateControllerGetFrameTime());
		if (AEInputCheckCurr(AEVK_A)) x -= spd;
		if (AEInputCheckCurr(AEVK_D)) x += spd;
		if (AEInputCheckCurr(AEVK_S)) y -= spd;
		if (AEInputCheckCurr(AEVK_W)) y += spd;

		float halfW = Config::SCREEN_WIDTH / 2.0f, halfH = Config::SCREEN_HEIGHT / 2.0f;

		float minX = Config::WORLD_MIN_X + halfW;
		float maxX = Config::WORLD_MAX_X - halfW;
		float minY = Config::WORLD_MIN_Y + halfH;
		float maxY = Config::WORLD_MAX_Y - halfH;

		if (x < minX) x = minX;
		if (x > maxX) x = maxX;
		if (y < minY) y = minY;
		if (y > maxY) y = maxY;

		AEGfxSetCamPosition(floorf(x), floorf(y));

		//Button btn = Button{ "Button", {0,0}, 100,100, Color{0,1,0}, texture };
		//GraphicsManager::DrawButton(btn);
	}

	void Exit() {

	}
}
