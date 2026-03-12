#include "LevelManager.hpp"

namespace LevelManager {
	Level* selectedLevel = nullptr; // current selected level
	std::vector<Level> levelDatas; // vector containing all the level data in the game

	void LoadAllLevels() {
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