#include "LevelManager.hpp"

namespace LevelManager {
	Level* selectedLevel = nullptr;
	std::vector<Level> levelDatas;

	void Load() {
		int levelCount = 1;
		for (int i = 0; i < levelCount; ++i) {
			std::string levelName = "level" + std::to_string(i + 1);
			Level data{ levelName };
			data.Load();
			levelDatas.push_back(data);
		}
	}

	void SelectLevel(int levelId) {
		selectedLevel = &levelDatas[levelId];
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
}