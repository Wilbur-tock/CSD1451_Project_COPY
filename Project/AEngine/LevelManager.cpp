#include "LevelManager.hpp"

namespace LevelManager {
	Level* selectedLevel = nullptr;
	std::vector<Level> levelDatas;
	int highestUnlockedLevel = FIRST_LEVEL_INDEX; // 0-based. Level 1 is unlocked by default.

	void LoadAllLevels() {
		selectedLevel = nullptr;
		levelDatas.clear();
		levelDatas.reserve(TOTAL_LEVEL_COUNT);

		for (int i = 0; i < TOTAL_LEVEL_COUNT; ++i) {
			Level data{ i };
			levelDatas.push_back(data);
			levelDatas[i].Load();
		}

		ResetProgress();
	}

	void LoadLevel(int levelId) {
		if (!IsValidLevelIndex(levelId)) return;
		levelDatas[levelId].Load();
	}

	void SelectLevel(int levelId) {
		if (!IsValidLevelIndex(levelId)) {
			selectedLevel = nullptr;
			return;
		}

		selectedLevel = &(levelDatas[levelId]);
	}

	Level* CurrentLevel() {
		return selectedLevel;
	}

	int LevelCount() {
		return TOTAL_LEVEL_COUNT;
	}

	bool IsValidLevelIndex(int levelIndex) {
		return levelIndex >= 0 && levelIndex < TOTAL_LEVEL_COUNT;
	}

	bool IsLevelUnlocked(int levelIndex) {
		return IsValidLevelIndex(levelIndex) && levelIndex <= highestUnlockedLevel;
	}

	int HighestUnlockedLevel() {
		return highestUnlockedLevel;
	}

	void UnlockLevel(int levelIndex) {
		if (!IsValidLevelIndex(levelIndex)) return;

		if (levelIndex > highestUnlockedLevel) {
			highestUnlockedLevel = levelIndex;
		}
	}

	void UnlockNextLevel(int completedLevelIndex) {
		UnlockLevel(completedLevelIndex + 1);
	}

	void ResetProgress() {
		highestUnlockedLevel = FIRST_LEVEL_INDEX;
	}

	void InitializeLevel() {
		if (selectedLevel) {
			selectedLevel->Initalize();
		}
	}

	void UpdateLevel() {
		if (selectedLevel) {
			selectedLevel->Update();
		}
	}

	void FreeLevel() {
		if (selectedLevel) {
			selectedLevel->Free();
		}
	}

	void UnloadLevel() {
		levelDatas.clear();
		selectedLevel = nullptr;
		highestUnlockedLevel = FIRST_LEVEL_INDEX;
	}
}