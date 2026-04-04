/*
Authors: Nixon
Description: Display credits information

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "Credits.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "Utils.hpp"
#include "TextObject.hpp"

namespace Credits {

    constexpr char const* CREDITS_FILE_PATH = "Assets/Data/credits.txt";

    // Path to the DigiPen logo shown at the very start of the credits scroll.
    constexpr char const* DIGIPEN_LOGO_PATH = "Assets/DigiPen_Logo.png";

    // =========================================================================
    // Entry type system
    //
    // Each line in credits.txt is classified by an optional prefix:
    //
    //   #  Title text        -> TITLE   (large, white)
    //   ## Section heading   -> SECTION (medium, gold)
    //   >  Role / label      -> ROLE    (small, grey, indented)
    //   !  Highlight note    -> HIGHLIGHT (normal size, tinted cyan)
    //   (blank line)         -> SPACER  (vertical gap, no text)
    //   anything else        -> BODY    (normal, white)
    //
    // To add a new type in the future:
    //   1. Add an enum value here.
    //   2. Add a prefix check in ParseLine().
    //   3. Add a StyleFor() entry.
    // =========================================================================

    enum class EntryType { TITLE, SECTION, ROLE, HIGHLIGHT, BODY, SPACER };

    struct CreditEntry {
        EntryType   type;
        std::string text;
    };

    // Visual style applied per entry type during Draw().
    struct EntryStyle {
        float textScale;  // passed to AEGfxPrint
        float r, g, b;    // text colour
        float xOffset;    // normalized X position (-1..1), used for indentation
        float lineHeight; // normalized Y step consumed by this entry
    };

    // StyleFor: the single place that controls how every type looks.
    static EntryStyle StyleFor(EntryType type) {
        switch (type) {
        case EntryType::TITLE:
            return { 1.1f,  1.0f, 1.0f, 1.0f,  -0.18f, 0.10f };

        case EntryType::SECTION:
            return { 0.85f, 1.0f, 0.85f, 0.2f, -0.28f, 0.08f };

        case EntryType::ROLE:
            return { 0.60f, 0.65f, 0.65f, 0.65f, -0.18f, 0.05f };

        case EntryType::HIGHLIGHT:
            return { 0.70f, 0.4f, 0.9f, 1.0f,  -0.28f, 0.06f };

        case EntryType::SPACER:
            return { 0.0f,  0.0f, 0.0f, 0.0f,  0.0f,   0.04f };

        case EntryType::BODY:
        default:
            return { 0.70f, 1.0f, 1.0f, 1.0f,  -0.28f, 0.055f };
        }
    }

    static CreditEntry ParseLine(std::string const& raw) {
        if (raw.empty()) {
            return { EntryType::SPACER, "" };
        }
        if (raw.size() > 3 && raw[0] == '#' && raw[1] == '#' && raw[2] == ' ') {
            return { EntryType::SECTION, raw.substr(3) };
        }
        if (raw.size() > 2 && raw[0] == '#' && raw[1] == ' ') {
            return { EntryType::TITLE, raw.substr(2) };
        }
        if (raw.size() > 2 && raw[0] == '>' && raw[1] == ' ') {
            return { EntryType::ROLE, raw.substr(2) };
        }
        if (raw.size() > 2 && raw[0] == '!' && raw[1] == ' ') {
            return { EntryType::HIGHLIGHT, raw.substr(2) };
        }
        return { EntryType::BODY, raw };
    }

    AEGfxTexture* bgTexture = nullptr;
    AEGfxTexture* logoTexture = nullptr;

    TextObject    hintText;

    float scrollOffset = 0.0f;
    float scrollSpeed = 0.3f;
    float totalHeight = 0.0f;

    // Increase for more breathing between the logo image and the first line
    constexpr float LOGO_BLOCK_HEIGHT = 0.55f;

    std::vector<CreditEntry> entries;


    void Load() {
        entries.clear();
        totalHeight = 0.0f;

        std::ifstream ifs{ CREDITS_FILE_PATH };
        if (!ifs.is_open()) {
            std::cerr << "[Credits] Could not open " << CREDITS_FILE_PATH
                << " -- using fallback.\n";
            entries.push_back({ EntryType::TITLE,   "GAME CREDITS" });
            entries.push_back({ EntryType::SPACER,  "" });
            entries.push_back({ EntryType::BODY,    "(credits.txt not found)" });
            entries.push_back({ EntryType::SPACER,  "" });
            entries.push_back({ EntryType::BODY,    "Press ESC to return to menu" });
        }
        else {
            std::string raw;
            while (std::getline(ifs, raw)) {
                entries.push_back(ParseLine(raw));
            }
        }

        // Include LOGO_BLOCK_HEIGHT in totalHeight so the auto-return trigger
        // in Update() fires only after the logo AND all text have scrolled off.
        totalHeight = LOGO_BLOCK_HEIGHT;
        for (CreditEntry const& e : entries) {
            totalHeight += StyleFor(e.type).lineHeight;
        }

        // Load logo through AssetsManager cache -- lifetime managed by AssetsManager.
        logoTexture = AssetsManager::GetTexture(DIGIPEN_LOGO_PATH);
    }

    void Initialize() {
        // Start below the bottom of the screen so everything scrolls in from below.
        scrollOffset = -1.0f;

        hintText = {
            "ESC - Return to Menu  |  SPACE - Skip",
            0.7f,
            Color(0.6f, 0.6f, 0.6f, 1.0f),
            { { -0.98f, -0.93f }, { 0.0f, 0.0f } },
        };
    }

    void Update() {
        scrollOffset += scrollSpeed * static_cast<float>(AEFrameRateControllerGetFrameTime());

        if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_BACK)) {
            GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
        }

        if (AEInputCheckTriggered(AEVK_SPACE)) {
            scrollOffset += 1.0f;
        }

        // Auto-return once everything (logo + all text) has scrolled off screen.
        if (scrollOffset > totalHeight + 1.0f) {
            GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
        }

        Draw();
    }

    void Draw() {
        // ------------------------------------------------------------------
        // Background
        // ------------------------------------------------------------------
        if (bgTexture != nullptr) {
            GraphicsManager::DrawWorldUI(
                bgTexture,
                GraphicsManager::CreateTransformMatrix(
                    0, 0,
                    Config::SCREEN_WIDTH,
                    Config::SCREEN_HEIGHT
                ),
                Color(1.0f, 1.0f, 1.0f, 1.0f)
            );
        }
        else {
            AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
        }

        // ------------------------------------------------------------------
        // DigiPen logo -- first item in the scroll stream
        //
        // Coordinate mapping:
        //   AEGfxPrint places text using normalized Y: -1 = bottom, +1 = top.
        //   DrawWorldUI uses world-space pixel Y, centered at the origin:
        //     worldY = normalizedY * (SCREEN_HEIGHT / 2)  =  normalizedY * 450
        //
        // The logo is treated as a virtual entry at normalized Y = scrollOffset
        // (the very first position in the stream, same anchor as the first text
        // line would have before LOGO_BLOCK_HEIGHT pushes text down).
        // Converting to world Y lets DrawWorldUI place it correctly.
        //
        // DrawWorldUI uses a top-left corner origin, so we subtract half the
        // logo height from the center world Y to visually centre the image on
        // the scroll anchor point.
        // ------------------------------------------------------------------

        if (logoTexture != nullptr) {
            // Display at 60% of screen width, preserving 1280:540 source ratio.
            float logoW = Config::SCREEN_WIDTH * 0.60f;      // 960 px
            float logoH = logoW * (540.0f / 1280.0f);        // ~405 px

            float halfScreen = Config::SCREEN_HEIGHT * 0.5f; // 450 px

            // The logo's center in world Y tracks scrollOffset directly.
            float logoCenterWorldY = scrollOffset * halfScreen;

            // Top-left Y for DrawWorldUI (world space, positive = up).
            float logoTopY = logoCenterWorldY + (logoH * 0.5f);

            // Cull when fully off screen to avoid unnecessary draw calls.
            bool logoOnScreen = (logoTopY > -halfScreen - logoH) &&
                (logoTopY < halfScreen + logoH);

            if (logoOnScreen) {
                float logoLeftX = -(logoW * 0.5f); // horizontally centered

                GraphicsManager::DrawWorldUI(
                    logoTexture,
                    GraphicsManager::CreateTransformMatrix(
                        logoLeftX, logoTopY,
                        logoW, logoH
                    ),
                    Color(1.0f, 1.0f, 1.0f, 1.0f)
                );
            }
        }

        // ------------------------------------------------------------------
        // Scrolling text entries
        //
        // Text begins LOGO_BLOCK_HEIGHT normalized units below the logo anchor
        // so the first line follows the logo naturally as both scroll upward.
        // ------------------------------------------------------------------

        float currentY = scrollOffset - LOGO_BLOCK_HEIGHT;

        for (CreditEntry const& entry : entries) {
            EntryStyle style = StyleFor(entry.type);

            bool onScreen = (currentY > -1.2f && currentY < 1.2f);
            if (entry.type != EntryType::SPACER && onScreen) {
                AEGfxPrint(
                    AssetsManager::textFont,
                    entry.text.c_str(),
                    style.xOffset,
                    currentY,
                    style.textScale,
                    style.r, style.g, style.b,
                    1.0f
                );
            }

            currentY -= style.lineHeight;
        }
        hintText.Draw();
    }

    void Free() {
        scrollOffset = 0.0f;
    }

    void Unload() {
        // Textures are owned by AssetsManager::textureCache; do not free here.
        logoTexture = nullptr;
    }

} // namespace Credits