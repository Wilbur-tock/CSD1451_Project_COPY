#ifndef GAMESTATEMANAGER_HPP
#define GAMESTATEMANAGER_HPP

#include "pch.hpp"
#include "Utils.hpp"
#include "CameraManager.hpp"

namespace GameStateManager {
	enum GameState {
		GS_INIT,
		GS_MAIN_MENU,
		GS_MAP_EDITOR,
		GS_LEVEL_1,
		GS_LEVEL_2,
		GS_CREDITS,
		GS_RESTART,
		GS_EXIT,
		GS_QUIT,
	};

	extern Utils::FuncPtr gsLoad, gsIntialize, gsUpdate, gsDraw, gsFree, gsUnload;
	extern GameState current, previous, next;

	extern float dt; // delta time

	void Load();

	void Update();

	void Exit();

	// Render loading screen during state transitions
	void RenderLoadingScreen();
}

#endif //