#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include "pch.hpp"
#include "Level.hpp"

namespace LevelManager {
	void LoadAllLevels();

	void LoadLevel(int levelIndex);
	void SelectLevel(int levelIndex);
	Level* CurrentLevel();

	int LevelCount();
	bool IsValidLevelIndex(int levelIndex);
	Level const* GetLevel(int levelIndex);
	std::string GetLevelName(int levelIndex);

	void InitializeLevel();
	void UpdateLevel();
	void FreeLevel();
	void UnloadLevel();
}

#endif // !LEVELMANAGER_HPP