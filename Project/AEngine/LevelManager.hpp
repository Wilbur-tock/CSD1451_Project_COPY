#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include "pch.hpp"
#include "Level.hpp"

namespace LevelManager {
	void LoadAllLevels(); // Load all levels data

	void LoadLevel(int levelId); // Load the level data based on the level index given
	void SelectLevel(int levelId); 
	Level* CurrentLevel();

	int LevelCount();
	bool IsValidLevelIndex(int levelIndex);
	bool IsLevelUnlocked(int levelIndex);
	int HighestUnlockedLevel();
	void UnlockLevel(int levelIndex);
	void UnlockNextLevel(int completedLevelIndex);
	void ResetProgress();

	void InitializeLevel();
	void UpdateLevel();
	void FreeLevel();
	void UnloadLevel();
}

#endif // !LEVELMANAGER_HPP