#include "pch.hpp"

#include "MapManager.hpp"
#include "MapEditor.hpp"
#include "CannonManager.hpp"

namespace GameStateManager {

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
