#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include "pch.hpp"
#include "Level.hpp"

namespace LevelManager {
	void LoadAllLevels(); // Load all the levels data (Map layout, all instance of player and enemy character object)

	void LoadLevel(int levelId);
	void SelectLevel(int levelId);
	Level* CurrentLevel();

	void InitializeLevel();
	void UpdateLevel();
	void FreeLevel();
	void UnloadLevel();
}

#endif // !LEVELMANAGER_HPP