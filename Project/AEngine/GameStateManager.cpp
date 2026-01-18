#include "pch.hpp"

#include "GameStateManager.hpp"
#include "GraphicsManager.hpp"
#include "MapManager.hpp"
#include "MapEditor.hpp"

namespace GameStateManager {

	AEGfxTexture* texture;

	void Load() {
		GraphicsManager::GenerateMesh();

		texture = AEGfxTextureLoad("Assets/square.png");
	}

	void Intialize() {

	}

	bool mapEditorOn = false;

	void Update() {

		if (AEInputCheckTriggered(AEVK_P)) {
			mapEditorOn = !mapEditorOn;
		}

		if(mapEditorOn) MapEditor::MapEditor();
	}

	void Draw() {

	}

	void Unload() {

	}

	void Exit() {

	}
}
