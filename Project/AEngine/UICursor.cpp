#include "UICursor.hpp"

#include "AssetsManager.hpp"
#include "GraphicsManager.hpp"
#include "Config.hpp"
#include "Color.hpp"

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif

namespace UICursor {

    static const char* CURSOR_PATH = "Assets/cursor.png";

    static AEGfxTexture* gCursorTex = nullptr;
    static AEVec2 gSize = { 50.0f, 50.0f };      // pixels
    static AEVec2 gHotspot = { 0.0f, 0.0f };     // pixels from TOP-LEFT of sprite
    static bool gVisible = true;

#ifdef _WIN32
    static void HideSystemCursor() {
        // ShowCursor uses an internal counter: loop until hidden
        while (ShowCursor(FALSE) >= 0) {}
        SetCursor(NULL);
    }

    static void ShowSystemCursor() {
        while (ShowCursor(TRUE) < 0) {}
    }
#endif

    void Load() {
        gCursorTex = AssetsManager::GetTexture(CURSOR_PATH);

#ifdef _WIN32
        HideSystemCursor();
#endif
    }

    void Unload() {
#ifdef _WIN32
        ShowSystemCursor();
#endif
        gCursorTex = nullptr;
    }

    void SetSize(AEVec2 sizePx) { gSize = sizePx; }
    void SetHotspot(AEVec2 hotspotPxFromTopLeft) { gHotspot = hotspotPxFromTopLeft; }
    void SetVisible(bool visible) { gVisible = visible; }

    void Draw() {
        if (!gVisible || !gCursorTex) return;

#ifdef _WIN32
        // Some systems reset the cursor; null it again to be safe
        SetCursor(NULL);
#endif

        // Mouse in screen pixels (top-left origin)
        s32 mx = 0, my = 0;
        AEInputGetCursorPosition(&mx, &my);

        // Convert to your engine screen-world coordinates (center origin, +Y up)
        float x = static_cast<float>(mx) - (Config::SCREEN_WIDTH * 0.5f);
        float y = (Config::SCREEN_HEIGHT * 0.5f) - static_cast<float>(my);

        // DrawScreenOverlay expects TOP-LEFT anchored transforms.
        // If hotspot is inside the sprite, offset so hotspot lands at (x,y).
        float originX = x - (gSize.x * 0.25f);
        float originY = y + (gSize.y * 0.25f);

        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(originX, originY, 0.0f, gSize.x, gSize.y);
        GraphicsManager::DrawScreenOverlay(gCursorTex, mat, Color{ 1.0f, 1.0f, 1.0f, 1.0f });
    }
}