#ifndef GAMESTATEMANAGER_HPP
#define GAMESTATEMANAGER_HPP

#include "pch.hpp"
#include "Utils.hpp"
#include "CameraManager.hpp"

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

	// IsGameplayState (private helper), returns true if state supports pausing
	static bool IsGameplayState() {
		return current == GameState::LEVEL_1
			|| current == GameState::LEVEL_2
			|| current == GameState::MAP_EDITOR;
	}

	void Load();

	void Update();

	void Exit();

}

#endif //