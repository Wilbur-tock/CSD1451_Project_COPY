/*
Authors: Dayrius, Bryan
Description: Manage all level data in the game

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "LevelManager.hpp"

#include "GameTrackingManager.hpp"

namespace LevelManager {
	constexpr char const* LEVEL_DATA_PATH = "Assets/Data/levelData.txt";

	Level* selectedLevel = nullptr;
	std::vector<Level> levelDatas;

	// Per-level win and loss counters.
	// Parallel to levelDatas; index matches level id.
	std::vector<int> levelWins;
	std::vector<int> levelLosses;

	// Campaign progression only.
	// Sandbox is handled separately and is always unlocked.
	int highestUnlockedCampaignLevel = Config::FIRST_CAMPAIGN_LEVEL_INDEX;

	bool IsSandboxLevel(int levelIndex) {
		return levelIndex == Config::SANDBOX_LEVEL_INDEX;
	}

	bool IsCampaignLevel(int levelIndex) {
		return levelIndex >= Config::FIRST_CAMPAIGN_LEVEL_INDEX &&
			levelIndex < Config::TOTAL_LEVEL_COUNT;
	}

	bool ComputeUnlockedState(int levelIndex) {
		if (IsSandboxLevel(levelIndex)) {
			return true;
		}

		return IsCampaignLevel(levelIndex) &&
			levelIndex <= highestUnlockedCampaignLevel;
	}

	void SaveLevelData() {
		std::ofstream ofs{ LEVEL_DATA_PATH };
		if (!ofs.is_open()) {
			std::cerr << "Can't open " << LEVEL_DATA_PATH << std::endl;
			return;
		}

		for (int i = 0; i < static_cast<int>(levelDatas.size()); ++i) {
			Level const& levelData = levelDatas[i];

			int wins = (i < static_cast<int>(levelWins.size())) ? levelWins[i] : 0;
			int losses = (i < static_cast<int>(levelLosses.size())) ? levelLosses[i] : 0;

			ofs << "Level" << i
				<< ":\nDifficulty: " << static_cast<int>(levelData.Difficulty())
				<< "\nIsUnlocked: " << static_cast<int>(ComputeUnlockedState(i))
				<< "\nWins: " << wins
				<< "\nLosses: " << losses
				<< "\n\n";
		}
	}

	void LoadAllLevels() {
		selectedLevel = nullptr;
		levelDatas.clear();
		levelDatas.reserve(Config::TOTAL_LEVEL_COUNT);

		// Reset progression baseline:
		// Sandbox is always unlocked, and the first campaign level is always unlocked.
		highestUnlockedCampaignLevel = Config::FIRST_CAMPAIGN_LEVEL_INDEX;

		// Reset win/loss counters; they will be overwritten from file if it exists.
		levelWins.assign(Config::TOTAL_LEVEL_COUNT, 0);
		levelLosses.assign(Config::TOTAL_LEVEL_COUNT, 0);

		// Load all the data levels and add into the vector
		for (int i = 0; i < Config::TOTAL_LEVEL_COUNT; ++i) {
			Level data{ i };
			levelDatas.push_back(data);
			levelDatas[i].Load();
		}

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
				int  wins = 0;
				int  losses = 0;

				ifs >> label >> diff
					>> label >> isUnlocked
					>> label >> wins
					>> label >> losses;

				levelDatas[id].Difficulty(diff);

				// Sandbox does not participate in progression tracking.
				// Only campaign levels update the saved progression state.
				if (IsCampaignLevel(id) && isUnlocked) {
					highestUnlockedCampaignLevel = (highestUnlockedCampaignLevel > id) ? highestUnlockedCampaignLevel : id;
				}

				levelWins[id] = wins;
				levelLosses[id] = losses;

				++id;
			}

			ifs.close();
		}
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
		if (!IsValidLevelIndex(levelIndex)) return false;

		// Sandbox is always open.
		if (IsSandboxLevel(levelIndex)) {
			return true;
		}

		return IsCampaignLevel(levelIndex) &&
			levelIndex <= highestUnlockedCampaignLevel;
	}

	int HighestUnlockedLevel() {
		return highestUnlockedCampaignLevel;
	}

	void UnlockLevel(int levelIndex) {
		// Only campaign levels should affect progression.
		if (!IsCampaignLevel(levelIndex)) return;

		if (levelIndex > highestUnlockedCampaignLevel) {
			highestUnlockedCampaignLevel = levelIndex;
		}
	}

	void UnlockNextLevel(int completedLevelIndex) {
		const int nextLevelIndex = completedLevelIndex + 1;

		// This automatically ignores invalid/final-next cases.
		UnlockLevel(nextLevelIndex);
	}

	void RecordWin(int levelIndex) {
		if (!IsValidLevelIndex(levelIndex)) return;
		++levelWins[levelIndex];
		SaveProgress(); // Persist immediately so the count survives a crash/quit.
	}

	void RecordLoss(int levelIndex) {
		if (!IsValidLevelIndex(levelIndex)) return;
		++levelLosses[levelIndex];
		SaveProgress();
	}

	int GetWins(int levelIndex) {
		if (!IsValidLevelIndex(levelIndex)) return 0;
		return levelWins[levelIndex];
	}

	int GetLosses(int levelIndex) {
		if (!IsValidLevelIndex(levelIndex)) return 0;
		return levelLosses[levelIndex];
	}

	void ResetProgress() {
		// Reset campaign only.
		// Sandbox remains always unlocked through IsLevelUnlocked().
		highestUnlockedCampaignLevel = Config::FIRST_CAMPAIGN_LEVEL_INDEX;
	}

	void SaveProgress() {
		if (levelDatas.empty()) {
			std::cerr << "No level data loaded. Progress was not saved.\n";
			return;
		}
		SaveLevelData();
	}

	void InitializeLevel() {
		if (selectedLevel) {
			selectedLevel->Initialize();
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
		SaveProgress();

		levelDatas.clear();
		selectedLevel = nullptr;
	}
}