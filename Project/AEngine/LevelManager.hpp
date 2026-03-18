#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include "pch.hpp"
#include "Level.hpp"

namespace LevelManager {
	void LoadAllLevels(); // Load all levels data (map layout, player/enemy objects)

	void LoadLevel(int levelId);
	void SelectLevel(int levelId);
	Level* CurrentLevel();

	void InitializeLevel();
	void UpdateLevel();
	void FreeLevel();
	void UnloadLevel();

	bool IsValidLevelId(int levelId);
}

#endif // !LEVELMANAGER_HPP