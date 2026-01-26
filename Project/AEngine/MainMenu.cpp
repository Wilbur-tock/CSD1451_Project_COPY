/* MainMenu.c */
/******************************************************************************/
/*!
\file       MainMenu.c
\brief      Main menu implementation for Alpha Engine games
*/
/******************************************************************************/

#include "MainMenu.hpp"
#include <stdio.h>
#include "AEEngine.h"
#include "GraphicsManager.hpp"

// Static helper function prototypes
static void MainMenuHandleKeyboard(MainMenu* pMenu);
static void MainMenuHandleMouse(MainMenu* pMenu);
static void MainMenuDrawButton(AEVec2 position, f32 width, f32 height, f32 r, f32 g, f32 b, f32 alpha);
static void MainMenuCreateButtonMesh(MainMenu* pMenu);

/******************************************************************************/
/*!
\fn         void MainMenuLoad(MainMenu* pMenu)
\brief      Load menu resources (fonts, meshes, etc.)
\param      [in,out] pMenu - Pointer to MainMenu structure
*/
/******************************************************************************/
void MainMenuLoad(MainMenu* pMenu)
{
    // Create fonts (adjust paths as needed)
    pMenu->fontTitle = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);
    pMenu->fontMenu = AEGfxCreateFont("Assets/liberation-mono.ttf", 48);

    // Fallback to system font if custom fonts not found
    //if (pMenu->fontTitle < 0)
    //{
    //    pMenu->fontTitle = AEGfxCreateFont("C:/Windows/Fonts/Arial.ttf", 72);
    //}
    //if (pMenu->fontMenu < 0)
    //{
    //    pMenu->fontMenu = AEGfxCreateFont("C:/Windows/Fonts/Arial.ttf", 48);
    //}

    // Create button mesh
    MainMenuCreateButtonMesh(pMenu);

    // Initialize menu state
    pMenu->selectedOption = MENU_START_GAME;
    pMenu->hoveredOption = (MenuOption)-1;
    pMenu->animationTimer = 0.0f;

    // Set up menu positions
    pMenu->titleY = 0.6f;

    f32 startY = 0.2f;
    f32 spacing = 0.25f;

    for (u32 i = 0; i < MENU_OPTION_COUNT; ++i)
    {
        AEVec2Set(&pMenu->menuPositions[i], 0.0f, startY - (i * spacing));
    }
}

/******************************************************************************/
/*!
\fn         void MainMenuInit(MainMenu* pMenu)
\brief      Initialize menu for new session
\param      [in,out] pMenu - Pointer to MainMenu structure
*/
/******************************************************************************/
void MainMenuInit(MainMenu* pMenu)
{
    // Set background color (dark blue)
    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.15f);

    // Reset to default selection
    pMenu->selectedOption = MENU_START_GAME;
    pMenu->hoveredOption = (MenuOption)-1;
    pMenu->animationTimer = 0.0f;
}

/******************************************************************************/
/*!
\fn         void MainMenuUpdate(MainMenu* pMenu)
\brief      Update menu logic and input
\param      [in,out] pMenu - Pointer to MainMenu structure
*/
/******************************************************************************/
void MainMenuUpdate(MainMenu* pMenu)
{
    // Update animation timer
    pMenu->animationTimer += (f32)AEFrameRateControllerGetFrameTime();

    // Handle input
    MainMenuHandleKeyboard(pMenu);
    MainMenuHandleMouse(pMenu);
}

/******************************************************************************/
/*!
\fn         void MainMenuDraw(MainMenu* pMenu)
\brief      Render the menu
\param      [in] pMenu - Pointer to MainMenu structure
*/
/******************************************************************************/
void MainMenuDraw(MainMenu* pMenu)
{
    const char* menuText[MENU_OPTION_COUNT] = {
        "Start Game",
        "Options",
        "Credits",
        "Quit"
    };

    // Draw title
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    // Calculate pulsing effect for title
    f32 titleScale = 1.5f + AESin(pMenu->animationTimer * 2.0f) * 0.05f;

    // Center the title text
    f32 titleWidth, titleHeight;
    AEGfxGetPrintSize(pMenu->fontTitle, "GAME MENU", titleScale,
        &titleWidth, &titleHeight);

    AEGfxPrint(pMenu->fontTitle, "GAME MENU",
        -titleWidth * 0.5f, pMenu->titleY, titleScale,
        1.0f, 0.8f, 0.2f, 1.0f);  // Gold color

    // Draw menu options
    for (u32 i = 0; i < MENU_OPTION_COUNT; ++i)
    {
        // Determine colors and scale based on state
        f32 red = 0.7f, green = 0.7f, blue = 0.7f;
        f32 scale = 1.0f;
        f32 bgAlpha = 0.0f;

        MenuOption option = static_cast<MenuOption>(i);

        if (pMenu->selectedOption == option)
        {
            // Selected option (yellow with subtle pulse)
            f32 pulse = 0.5f + 0.5f * AESin(pMenu->animationTimer * 4.0f);
            red = 1.0f;
            green = 1.0f;
            blue = 0.3f + pulse * 0.2f;
            scale = 1.15f;
            bgAlpha = 0.3f;
        }
        else if (pMenu->hoveredOption == option)
        {
            // Hovered option (light)
            red = 0.9f;
            green = 0.9f;
            blue = 0.9f;
            scale = 1.1f;
            bgAlpha = 0.2f;
        }

        // Calculate text dimensions for centering
        f32 textWidth, textHeight;
        AEGfxGetPrintSize(pMenu->fontMenu, menuText[i], scale,
            &textWidth, &textHeight);

        f32 posX = pMenu->menuPositions[i].x - (textWidth * 0.5f);
        f32 posY = pMenu->menuPositions[i].y;

        // Draw button background if highlighted
        if (bgAlpha > 0.0f && pMenu->pButtonMesh)
        {
            //MainMenuDrawButton(pMenu->menuPositions[i],
            //    textWidth + 0.2f, textHeight + 0.1f,
            //    red, green, blue, bgAlpha);
        }

        // Draw text
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxPrint(pMenu->fontMenu, menuText[i], posX, posY, scale,
            red, green, blue, 1.0f);
    }

    // Draw instructions at bottom
    AEGfxPrint(pMenu->fontMenu, "Use Arrow Keys or Mouse",
        -0.4f, -0.8f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.7f);
}

/******************************************************************************/
/*!
\fn         void MainMenuFree(MainMenu* pMenu)
\brief      Free runtime resources
\param      [in,out] pMenu - Pointer to MainMenu structure
*/
/******************************************************************************/
void MainMenuFree(MainMenu* pMenu)
{
    // Free button mesh
    if (pMenu->pButtonMesh)
    {
        AEGfxMeshFree(pMenu->pButtonMesh);
        pMenu->pButtonMesh = NULL;
    }
}

/******************************************************************************/
/*!
\fn         void MainMenuUnload(MainMenu* pMenu)
\brief      Unload all menu resources
\param      [in,out] pMenu - Pointer to MainMenu structure
*/
/******************************************************************************/
void MainMenuUnload(MainMenu* pMenu)
{
    // Destroy fonts
    if (pMenu->fontTitle >= 0)
    {
        AEGfxDestroyFont(pMenu->fontTitle);
        pMenu->fontTitle = -1;
    }

    if (pMenu->fontMenu >= 0)
    {
        AEGfxDestroyFont(pMenu->fontMenu);
        pMenu->fontMenu = -1;
    }
}

/******************************************************************************/
/*!
\fn         void MainMenuExecuteOption(MenuOption option)
\brief      Execute the selected menu option
\param      [in] option - The menu option to execute
*/
/******************************************************************************/
void MainMenuExecuteOption(MenuOption option)
{
    switch (option)
    {
    case MENU_START_GAME:
        printf("Starting game...\n");
        // TODO: Transition to gameplay state
        // Example: GameStateManager_ChangeState(GS_GAMEPLAY);
        break;

    case MENU_OPTIONS:
        printf("Opening options...\n");
        // TODO: Transition to options state
        // Example: GameStateManager_ChangeState(GS_OPTIONS);
        break;

    case MENU_CREDITS:
        printf("Opening credits...\n");
        // TODO: Transition to credits state
        // Example: GameStateManager_ChangeState(GS_CREDITS);
        break;

    case MENU_QUIT:
        printf("Quitting game...\n");
        // TODO: Proper cleanup and exit
        // Example: GameStateManager_ChangeState(GS_QUIT);
        exit(0);
        break;

    default:
        break;
    }
}

/******************************************************************************/
// STATIC HELPER FUNCTIONS
/******************************************************************************/

/******************************************************************************/
/*!
\fn         static void MainMenuHandleKeyboard(MainMenu* pMenu)
\brief      Handle keyboard input for menu navigation
\param      [in,out] pMenu - Pointer to MainMenu structure
*/
/******************************************************************************/
static void MainMenuHandleKeyboard(MainMenu* pMenu)
{
    // Navigate down
    if (AEInputCheckTriggered(AEVK_DOWN) || AEInputCheckTriggered(AEVK_S))
    {
        pMenu->selectedOption = (MenuOption)((pMenu->selectedOption + 1) % MENU_OPTION_COUNT);
        pMenu->hoveredOption = (MenuOption)-1;  // Clear hover when using keyboard
    }

    // Navigate up
    if (AEInputCheckTriggered(AEVK_UP) || AEInputCheckTriggered(AEVK_W))
    {
        pMenu->selectedOption = (MenuOption)((pMenu->selectedOption - 1 + MENU_OPTION_COUNT)
            % MENU_OPTION_COUNT);
        pMenu->hoveredOption = (MenuOption)-1;  // Clear hover when using keyboard
    }

    // Select with Enter or Space
    if (AEInputCheckTriggered(AEVK_RETURN) || AEInputCheckTriggered(AEVK_SPACE))
    {
        MainMenuExecuteOption(pMenu->selectedOption);
    }

    // Quick quit with Escape
    if (AEInputCheckTriggered(AEVK_ESCAPE))
    {
        MainMenuExecuteOption(MENU_QUIT);
    }
}

/******************************************************************************/
/*!
\fn         static void MainMenuHandleMouse(MainMenu* pMenu)
\brief      Handle mouse input for menu interaction
\param      [in,out] pMenu - Pointer to MainMenu structure
*/
/******************************************************************************/
static void MainMenuHandleMouse(MainMenu* pMenu)
{
    // Get mouse position
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    // Convert to normalized coordinates
    s32 windowWidth = AEGfxGetWindowWidth();
    s32 windowHeight = AEGfxGetWindowHeight();

    f32 normalizedX = (mouseX / (f32)windowWidth) * 2.0f - 1.0f;
    f32 normalizedY = -((mouseY / (f32)windowHeight) * 2.0f - 1.0f);

    // Check hover state
    pMenu->hoveredOption = (MenuOption)-1;

    f32 hoverHeightRadius = 0.12f;
    f32 hoverWidthRadius = 0.6f;

    for (u32 i = 0; i < MENU_OPTION_COUNT; ++i)
    {
        f32 distY = fabsf(normalizedY - pMenu->menuPositions[i].y);
        f32 distX = fabsf(normalizedX - pMenu->menuPositions[i].x);

        if (distY < hoverHeightRadius && distX < hoverWidthRadius)
        {
            pMenu->hoveredOption = (MenuOption)i;
            pMenu->selectedOption = (MenuOption)i;
            break;
        }
    }

    // Execute on left click
    if (AEInputCheckTriggered(AEVK_LBUTTON))
    {
        if (pMenu->hoveredOption != (MenuOption)-1)
        {
            MainMenuExecuteOption(pMenu->hoveredOption);
        }
    }
}

/******************************************************************************/
/*!
\fn         static void MainMenuDrawButton(...)
\brief      Draw a button background rectangle
*/
/******************************************************************************/
static void MainMenuDrawButton(AEVec2 position, f32 width, f32 height,
    f32 r, f32 g, f32 b, f32 alpha)
{
    if (!width || !height) return;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetColorToMultiply(r, g, b, alpha);
    AEGfxSetTransparency(alpha);

    // Create transform matrix
    AEMtx33 scale, translate, transform;
    AEMtx33Scale(&scale, width, height);
    AEMtx33Trans(&translate, position.x, position.y);
    AEMtx33Concat(&transform, &scale, &translate);

    AEGfxSetTransform(transform.m);
}

/******************************************************************************/
/*!
\fn         static void MainMenuCreateButtonMesh(MainMenu* pMenu)
\brief      Create mesh for button backgrounds
\param      [in,out] pMenu - Pointer to MainMenu structure
*/
/******************************************************************************/
static void MainMenuCreateButtonMesh(MainMenu* pMenu)
{
    // Create a simple quad mesh for buttons
    AEGfxMeshStart();

    // Define quad vertices (counter-clockwise)
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 1.0f);

    AEGfxTriAdd(
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
        0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 1.0f);

    pMenu->pButtonMesh = AEGfxMeshEnd();
    AE_ASSERT_MESG(pMenu->pButtonMesh, "Failed to create button mesh!");
}

