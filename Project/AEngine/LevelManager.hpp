#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include "pch.hpp"
#include "Level.hpp"

namespace LevelManager {
	// current selected level
	extern Level* selectedLevel;
	static std::vector<Level> levelDatas; // vector containing all the level data in the game

	void Load(); // Load all the levels data (Map layout, all instance of player and enemy character object)
	void SelectLevel(int levelId); // Select level

	void LoadLevel();
	void InitializeLevel();
	void UpdateLevel();
	void FreeLevel();
}

#endif // !LEVELMANAGER_HPP
