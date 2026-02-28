#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include "pch.hpp"
#include "Level.hpp"

namespace LevelManager {
	// current selected level
	extern Level* selectedLevel;
	extern std::vector<Level> levelDatas; // vector containing all the level data in the game

	void Load(); // Load all the levels in the game
	void SelectLevel(int levelId); // Select level

	void LoadLevel();
	void InitializeLevel();
	void UpdateLevel();
	void FreeLevel();
}

#endif // !LEVELMANAGER_HPP
