/*
Authors: Dayrius, Bryan
Description: Manage all level data in the game

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include "pch.hpp"
#include "Level.hpp"

namespace LevelManager {
	void LoadAllLevels(); // Load all levels data

	void LoadLevel(int levelId); // Load the level data based on the level index given
	void SelectLevel(int levelId);
	Level* CurrentLevel();

	bool IsValidLevelIndex(int levelIndex);
	bool IsLevelUnlocked(int levelIndex);
	int HighestUnlockedLevel();
	void UnlockLevel(int levelIndex);
	void UnlockNextLevel(int completedLevelIndex);
	void ResetProgress();
	void SaveProgress();

	// Win/loss record for a specific level (persisted to levelData.txt).
	// RecordWin/RecordLoss should be called at most once per completed match
	// (guarded by the hasProcessedVictory / hasProcessedLoss flags in Level).
	void RecordWin(int levelIndex);
	void RecordLoss(int levelIndex);
	int  GetWins(int levelIndex);
	int  GetLosses(int levelIndex);

	void InitializeLevel();
	void UpdateLevel();
	void FreeLevel();
	void UnloadLevel();
}

#endif // !LEVELMANAGER_HPP