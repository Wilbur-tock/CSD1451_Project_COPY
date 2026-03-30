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

	void InitializeLevel();
	void UpdateLevel();
	void FreeLevel();
	void UnloadLevel();
}

#endif // !LEVELMANAGER_HPP