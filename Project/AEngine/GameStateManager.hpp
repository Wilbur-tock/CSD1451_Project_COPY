#ifndef GAMESTATEMANAGER_HPP
#define GAMESTATEMANAGER_HPP

#include "pch.hpp"

namespace GameStateManager {

	using FP = void(*)(void);

	extern FP fpLoad, fpInitalize, fpUpdate, fpDraw, fpFree, fpUnload;
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