/* MainMenu.h */
/******************************************************************************/
/*!
\file       MainMenu.h
\brief      Main menu implementation for Alpha Engine games
\details    Provides a complete, functional main menu with keyboard and mouse
            support, visual feedback, and easy integration
*/
/******************************************************************************/

#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "AEEngine.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // Menu option enumeration
    typedef enum MenuOption
    {
        MENU_START_GAME = 0,
        MENU_OPTIONS,
        MENU_CREDITS,
        MENU_QUIT,
        MENU_OPTION_COUNT
    } MenuOption;

    // Main menu structure
    typedef struct MainMenu
    {
        s8 fontTitle;                              // Font ID for title
        s8 fontMenu;                               // Font ID for menu items
        MenuOption selectedOption;                 // Currently selected option
        MenuOption hoveredOption;                  // Option under mouse cursor
        AEVec2 menuPositions[MENU_OPTION_COUNT];  // Screen positions
        f32 titleY;                                // Title Y position
        AEGfxVertexList* pButtonMesh;             // Button background mesh
        f32 animationTimer;                        // For animations
    } MainMenu;

    // Function prototypes
    void MainMenuLoad(MainMenu* pMenu);
    void MainMenuInit(MainMenu* pMenu);
    void MainMenuUpdate(MainMenu* pMenu);
    void MainMenuDraw(MainMenu* pMenu);
    void MainMenuFree(MainMenu* pMenu);
    void MainMenuUnload(MainMenu* pMenu);
    void MainMenuExecuteOption(MenuOption option);

#ifdef __cplusplus
}
#endif

#endif // MAIN_MENU_H