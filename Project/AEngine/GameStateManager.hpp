#ifndef GAMESTATEMANAGER_HPP
#define GAMESTATEMANAGER_HPP

#include "pch.hpp"
#include "Utils.hpp"

namespace GameStateManager {
	enum class GameState {
		INITIALIZE,
		MAIN_MENU,
		MAP_EDITOR,
		LEVEL_1,
		LEVEL_2,
		CREDITS,
		SETTINGS,
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