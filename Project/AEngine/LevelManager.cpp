#include "LevelManager.hpp"

#include "GameTrackingManager.hpp"

namespace LevelManager {
	Level* selectedLevel = nullptr;
	std::vector<Level> levelDatas;
	int highestUnlockedLevel = Config::FIRST_LEVEL_INDEX;

	void LoadAllLevels() {
		selectedLevel = nullptr;
		levelDatas.clear();
		levelDatas.reserve(Config::TOTAL_LEVEL_COUNT);

		// Load all the data levels and add into the vector
		for (int i = 0; i < Config::TOTAL_LEVEL_COUNT; ++i) {
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
		return Config::TOTAL_LEVEL_COUNT;
	}

	bool IsValidLevelIndex(int levelIndex) {
		return levelIndex >= 0 && levelIndex < Config::TOTAL_LEVEL_COUNT;
	}

	bool IsLevelUnlocked(int levelIndex) {
		if (Utils::isDebuggerOn) return true; // unlock all levels if debugger is on
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
		highestUnlockedLevel = Config::FIRST_LEVEL_INDEX;
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
	}
}