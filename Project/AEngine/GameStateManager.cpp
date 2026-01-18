#include "pch.hpp"

#include "GameStateManager.hpp"
#include "GraphicsManager.hpp"
#include "MapManager.hpp"
#include "MapEditor.hpp"

namespace GameStateManager {

	AEGfxTexture* texture;

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
	}

	void Draw() {

	}

	void Unload() {

	}

	void Exit() {

	}
}
