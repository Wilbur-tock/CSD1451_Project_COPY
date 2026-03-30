#include "LevelManager.hpp"

#include "GameTrackingManager.hpp"

namespace LevelManager {
	char const* LEVEL_DATA_PATH = "Assets/Data/levelData.txt";

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

		// Fallback unlock state from level data file (for backward compatibility)
		int fallbackHighestUnlockedLevel = Config::FIRST_LEVEL_INDEX;

		std::ifstream ifs{ LEVEL_DATA_PATH };
		if (!ifs.is_open()) {
			std::cerr << "Can't open " << LEVEL_DATA_PATH << std::endl;
		}
		else {
			int id = 0;
			std::string levelName = "";

			while (ifs >> levelName && id < Config::TOTAL_LEVEL_COUNT) {
				std::string label = "";
				int diff = 0;
				bool isUnlocked = false;

				ifs >> label >> diff
					>> label >> isUnlocked;

				levelDatas[id].Difficulty(diff);

				if (isUnlocked) {
					fallbackHighestUnlockedLevel = id;
				}

				++id;
			}

			ifs.close();
		}

		highestUnlockedLevel = fallbackHighestUnlockedLevel;
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
		// Save level data file
		std::ofstream ofs{ LEVEL_DATA_PATH };
		if (!ofs.is_open()) {
			std::cerr << "Can't open " << LEVEL_DATA_PATH << std::endl;
		}
		else {
			for (int i = 0; i < static_cast<int>(levelDatas.size()); ++i) {
				Level const& levelData = levelDatas[i];
				ofs << "level" + std::to_string(i)
					<< "\nDifficulty: " << static_cast<int>(levelData.Difficulty())
					<< "\nIsUnlocked: " << static_cast<int>(levelData.Id() <= highestUnlockedLevel)
					<< "\n\n";
			}

			ofs.close();
		}

		levelDatas.clear();
		selectedLevel = nullptr;
	}
}