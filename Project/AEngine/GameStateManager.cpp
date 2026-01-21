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
		GraphicsManager::GenerateMesh();

		texture = AssetsManager::GetTexture("Assets/square.png");

		MapManager::LoadAllMapData();
	}

	void Intialize() {
	}

	bool mapEditorOn = false;

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

		if(mapEditorOn) MapEditor::MapEditor();

		MapManager::RenderMap(texture, MapManager::mapLayout);

		cannon.Update();
	}

	void Draw() {

	}

	void Unload() {

	}

	void Exit() {

	}
}
