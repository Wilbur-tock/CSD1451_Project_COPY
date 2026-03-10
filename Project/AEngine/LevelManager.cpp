#include "LevelManager.hpp"

namespace LevelManager {
	Level* selectedLevel = nullptr;

	void Load() {
		int levelCount = 1;
		levelDatas.clear();
		levelDatas.reserve(levelCount);

		// Add all instance of level into the vector
		for (int i = 0; i < levelCount; ++i) {
			int id = i + 1;
			Level data{ id };
			levelDatas.push_back(data);
			levelDatas[i].Load();
		}
	}

	void SelectLevel(int levelId) {
		selectedLevel = &(levelDatas[levelId]);
	}

	void LoadLevel() {
		//selectedLevel->Load();
	}

	void InitializeLevel() {
		selectedLevel->Initalize();
	}

	void UpdateLevel() {
		selectedLevel->Update();
	}

	void FreeLevel() {
		selectedLevel->Free();
	}

	void UnloadLevel() {
		levelDatas.clear();
		selectedLevel = nullptr;
	}
}