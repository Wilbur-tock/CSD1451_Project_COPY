#include "LevelManager.hpp"

namespace LevelManager {
	Level* selectedLevel = nullptr;

	void Load() {
		int levelCount = 1;
		levelDatas.clear();
		levelDatas.reserve(levelCount);

		for (int i = 0; i < levelCount; ++i) {
			std::string levelName = "level" + std::to_string(i + 1);
			Level data{ levelName };
			levelDatas.push_back(data);
			levelDatas[i].Load();
		}
	}

	void SelectLevel(int levelId) {
		selectedLevel = &(levelDatas[levelId]);
	}

	void LoadLevel() {
		selectedLevel->Load();
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

	void Unload() {
		selectedLevel = nullptr;
	}
}