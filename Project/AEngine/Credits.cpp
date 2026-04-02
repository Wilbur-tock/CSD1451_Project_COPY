/*
Authors: Nixon
Description: Display credits information

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "Credits.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "Utils.hpp"
#include "TextObject.hpp"

namespace Credits {

    // =========================================================================
    // File path
    // =========================================================================

    constexpr char const* CREDITS_FILE_PATH = "Assets/Data/credits.txt";

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
    //   That is all -- no other code needs to change.
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
    // Change a number here and every line of that type updates automatically.
    static EntryStyle StyleFor(EntryType type) {
        switch (type) {
        case EntryType::TITLE:
            // Large, centred-ish, pure white -- grabs attention at the top.
            return { 1.1f,  1.0f, 1.0f, 1.0f,  -0.18f, 0.10f };

        case EntryType::SECTION:
            // Gold accent, slightly smaller than title -- marks major groups.
            return { 0.85f, 1.0f, 0.85f, 0.2f, -0.28f, 0.08f };

        case EntryType::ROLE:
            // Dimmed grey, indented right relative to body -- sits above a name.
            return { 0.60f, 0.65f, 0.65f, 0.65f, -0.18f, 0.05f };

        case EntryType::HIGHLIGHT:
            // Cyan tint -- draws the eye to important notes or announcements.
            return { 0.70f, 0.4f, 0.9f, 1.0f,  -0.28f, 0.06f };

        case EntryType::SPACER:
            // No text drawn; lineHeight controls the gap size.
            return { 0.0f,  0.0f, 0.0f, 0.0f,  0.0f,   0.04f };

        case EntryType::BODY:
        default:
            // Plain white at normal scale -- the baseline for names and prose.
            return { 0.70f, 1.0f, 1.0f, 1.0f,  -0.28f, 0.055f };
        }
    }

    // =========================================================================
    // Parser
    //
    // ParseLine reads the prefix of a raw string and returns a CreditEntry with
    // the prefix stripped from the text. Blank lines become SPACER entries.
    //
    // The prefix table lives here. Adding a new marker is one new else-if.
    // =========================================================================

    static CreditEntry ParseLine(std::string const& raw) {
        // Blank line -> spacer (no text needed)
        if (raw.empty()) {
            return { EntryType::SPACER, "" };
        }

        // ## before # so the longer prefix is checked first
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

    // =========================================================================
    // State
    // =========================================================================

    AEGfxTexture* bgTexture = nullptr;
    TextObject    hintText;

    float scrollOffset = 0.0f;
    float scrollSpeed = 0.3f;
    float totalHeight = 0.0f; // sum of all line heights; computed once in Load()

    // Parsed entries are populated in Load() so the file is read only once per
    // session, not every time the Credits state is entered.
    std::vector<CreditEntry> entries;

    // =========================================================================
    // Load
    // =========================================================================

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
            entries.push_back({ EntryType::BODY,    "Press M to return to menu" });
        }
        else {
            std::string raw;
            while (std::getline(ifs, raw)) {
                entries.push_back(ParseLine(raw));
            }
        }

        // Pre-compute total scroll height so Update() knows when the credits
        // have fully scrolled past and can return to the main menu automatically.
        for (CreditEntry const& e : entries) {
            totalHeight += StyleFor(e.type).lineHeight;
        }
    }

    // =========================================================================
    // Initialize
    // =========================================================================

    void Initialize() {
        // Start below the bottom of the screen so credits scroll in from below.
        scrollOffset = -1.0f;

        hintText = {
            "M - Return to Menu  |  SPACE - Skip",
            0.7f,
            Color(0.6f, 0.6f, 0.6f, 1.0f),
            { { -0.98f, -0.93f }, { 0.0f, 0.0f } },
        };
    }

    // =========================================================================
    // Update
    // =========================================================================

    void Update() {
        scrollOffset += scrollSpeed * static_cast<float>(AEFrameRateControllerGetFrameTime());

        if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_BACK)) {
            GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
        }

        // SPACE skips forward by a large chunk so the player can jump to the end.
        if (AEInputCheckTriggered(AEVK_SPACE)) {
            scrollOffset += 1.0f;
        }

        // Once all entries have scrolled past the top of the screen, return to
        // the main menu. The +1.0f gives a small pause at the very end before
        // the transition fires, so the last line has time to clear the screen.
        if (scrollOffset > totalHeight + 1.0f) {
            GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
        }

        Draw();
    }

    // =========================================================================
    // Draw
    // =========================================================================

    void Draw() {
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
            AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
        }

        // Walk the entry list, drawing each line at its current scrolled Y.
        // currentY starts at scrollOffset (the global scroll position) and
        // steps downward by each entry's lineHeight as we go.
        float currentY = scrollOffset;

        for (CreditEntry const& entry : entries) {
            EntryStyle style = StyleFor(entry.type);

            // Only bother calling AEGfxPrint for entries that actually have text
            // and are somewhere near the visible screen range (-1..1 in Y).
            bool onScreen = (currentY > -1.2f && currentY < 1.2f);
            if (entry.type != EntryType::SPACER && onScreen) {
                AEGfxPrint(
                    AssetsManager::textFont,
                    entry.text.c_str(),
                    style.xOffset,
                    currentY,
                    style.textScale,
                    style.r, style.g, style.b,
                    1.0f  // alpha -- AEGfxPrint requires all 9 arguments
                );
            }

            currentY -= style.lineHeight;
        }

        hintText.Draw();
    }

    // =========================================================================
    // Free / Unload
    // =========================================================================

    void Free() {
        scrollOffset = 0.0f;
    }

    void Unload() {
        // entries and totalHeight are kept alive across revisits to Credits so
        // the file does not need to be re-read. The memory cost is negligible.
    }

} // namespace Credits