// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "GameStateManager.hpp"
#include "MainMenu.hpp"

// ---------------------------------------------------------------------------
// main
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	int gGameRunning = 1;

	// Using custom window procedure
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);

	// Changing the window title
	AESysSetWindowTitle("Game Name");

	// reset the system modules
	/*AESysReset();*/

	/*GameStateManager::Load();*/


	// Create and initialize menu
	MainMenu menu;
	MainMenuLoad(&menu);
	MainMenuInit(&menu);

	// Game Loop
	while (gGameRunning)
	{
		// Informing the system about the loop's start
		AESysFrameStart();
		AEInputUpdate();

		AEGfxSetBackgroundColor(0, 0, 0);

		// Basic way to trigger exiting the application
		// when ESCAPE is hit or when the window is closed
		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			gGameRunning = 0;

		// Your own update logic goes here
	/*	GameStateManager::Update();*/
		MainMenuUpdate(&menu);

		// Your own rendering logic goes here
		/*AEGfxStart();*/ //Epilepsy
		MainMenuDraw(&menu);
		/*AEGfxEnd();*/ //Epilepsy

		// Informing the system about the loop's end
		AESysFrameEnd();
	}

	GameStateManager::Exit();
	// free the system
	MainMenuFree(&menu);
	MainMenuUnload(&menu);
	AESysExit();
}
