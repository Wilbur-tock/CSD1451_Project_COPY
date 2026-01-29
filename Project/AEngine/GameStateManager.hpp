#ifndef GAMESTATEMANAGER_HPP
#define GAMESTATEMANAGER_HPP

#include "pch.hpp"
#include "Utils.hpp"
#include "CameraManager.hpp"

namespace GameStateManager {
	extern Utils::FuncPtr gsLoad, gsIntialize, gsUpdate, gsDraw, gsFree, gsUnload;
	extern int current, previous, next;

	enum GSList {
		GS_Main_Menu,
		GS_Map_Editor,
		GS_Level_1,
		GS_Level_2,
		GS_Restart,
		GS_QUIT,
		GS_Exit
	};

	void Load();

	void Update();

	void Exit();
}

#endif //