#include "LevelManager.hpp"

namespace LevelManager {
	namespace {
		constexpr int LEVEL_COUNT = 2;
	}

	Level* selectedLevel = nullptr;      // current selected level
	std::vector<Level> levelDatas;       // vector containing all the level data in the game

	bool IsValidLevelId(int levelId) {
		return levelId >= 0 && levelId < static_cast<int>(levelDatas.size());
	}

	void LoadAllLevels() {
		selectedLevel = nullptr;
		levelDatas.clear();
		levelDatas.reserve(LEVEL_COUNT);

		// Create and load all levels that can be selected from Level Select
		for (int i = 0; i < LEVEL_COUNT; ++i) {
			int id = i + 1; // level1.txt, level2.txt
			levelDatas.emplace_back(id);
			levelDatas.back().Load();
		}
	}

	void LoadLevel(int levelId) {
		if (!IsValidLevelId(levelId)) {
			std::cerr << "[LevelManager] Invalid level index in LoadLevel(): " << levelId << '\n';
			return;
		}

		levelDatas[levelId].Load();
	}

	void SelectLevel(int levelId) {
		if (!IsValidLevelId(levelId)) {
			std::cerr << "[LevelManager] Invalid level index in SelectLevel(): " << levelId << '\n';
			selectedLevel = nullptr;
			return;
		}

		selectedLevel = &levelDatas[levelId];
	}

	Level* CurrentLevel() {
		return selectedLevel;
	}

	void InitializeLevel() {
		if (!selectedLevel) {
			std::cerr << "[LevelManager] Cannot initialize level: no level selected.\n";
			return;
		}

		selectedLevel->Initalize();
	}

	void UpdateLevel() {
		if (!selectedLevel) {
			std::cerr << "[LevelManager] Cannot update level: no level selected.\n";
			return;
		}

		selectedLevel->Update();
	}

	void FreeLevel() {
		if (!selectedLevel) {
			return;
		}

		selectedLevel->Free();
	}

	void UnloadLevel() {
		selectedLevel = nullptr;
		levelDatas.clear();
	}
}