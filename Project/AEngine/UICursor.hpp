/*
Authors: Bryan
Description: Change the cursor to the game cursor

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef UI_UICURSOR_HPP
#define UI_UICURSOR_HPP

#include "AEEngine.h" // AEVec2

namespace UICursor {
    void Load();     // load texture + hide system cursor
    void Unload();   // show system cursor again
    void Draw();     // draw cursor on top of everything each frame

    // Optional tweaks:
    void SetSize(AEVec2 sizePx);                 // cursor size in pixels
    void SetHotspot(AEVec2 hotspotPxFromTopLeft);// where clicks "point" inside the sprite
    void SetVisible(bool visible);
}

#endif // UI_UICURSOR_HPP