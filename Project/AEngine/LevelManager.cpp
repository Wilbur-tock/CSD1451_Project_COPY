#include "LevelManager.hpp"

namespace LevelManager {
	Level* selectedLevel = nullptr;
	std::vector<Level> levelDatas;

	void LoadAllLevels() {
		int levelCount = 2;

		selectedLevel = nullptr;
		levelDatas.clear();
		levelDatas.reserve(levelCount);

		for (int i = 0; i < levelCount; ++i) {
			int id = i + 1;
			Level data{ id };
			levelDatas.push_back(data);
			levelDatas[i].Load();
		}
	}

	void LoadLevel(int levelId) {
		levelDatas[levelId].Load();
	}

	void SelectLevel(int levelId) {
		selectedLevel = &(levelDatas[levelId]);
	}

	Level* CurrentLevel() {
		return selectedLevel;
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