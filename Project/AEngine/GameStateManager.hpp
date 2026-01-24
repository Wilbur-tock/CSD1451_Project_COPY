#ifndef GAMESTATEMANAGER_HPP
#define GAMESTATEMANAGER_HPP

#include "pch.hpp"

namespace GameStateManager {

	using FP = void(*)(void);

	extern FP fpLoad, fpInitalize, fpUpdate, fpDraw, fpUnload, fpExit;
	extern int current, previous, next;

	enum GS_LIST {
		GS_Main_Menu,
		GS_Level_1,
		GS_Level_2,
	};

	void Load();

	void Update();

	void Exit();
}

#endif // 