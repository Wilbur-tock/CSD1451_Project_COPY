/*
Authors: Dayrius, Nixon, Bryan
Description: Manage all the game states in the game and transition between each states

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef GAMESTATEMANAGER_HPP
#define GAMESTATEMANAGER_HPP

#include "pch.hpp"
#include "Utils.hpp"

namespace GameStateManager {
	enum class GameState {
		INITIALIZE,
		SPLASH,      // DigiPen logo shown once on first launch
		MAIN_MENU,
		LEVEL_SELECT,
		MAP_EDITOR,
		LEVEL_0,
		LEVEL_1,
		LEVEL_2,
		LEVEL_3,
		CREDITS,
		SETTINGS,
		GUIDE,
		RESTART,
		EXIT,
		QUIT,
	};

	extern Utils::FuncPtr gsLoad, gsIntialize, gsUpdate, gsDraw, gsFree, gsUnload;
	extern GameState current, previous, next;

	void Load();
	void Update();
	void Exit();
}

#endif //