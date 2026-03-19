#include "Guide.hpp"

#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "CameraManager.hpp"
#include "UIManager.hpp"
#include "AudioManager.hpp"
#include "Color.hpp"
#include "Utils.hpp"
#include "UICursor.hpp"
#include "Config.hpp"
#include "Button.hpp"
#include "TextObject.hpp"
#include "json.hpp"

#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

namespace Guide {

    void OnBackClicked();
    void OnGameGuideClicked();
    void OnAbilitiesGuideClicked();

    namespace {
        enum class GuideView {
            MENU,
            GAME,
            ABILITIES_OFFENSIVE,
            ABILITIES_DEFENSIVE
        };

        struct AbilityRow {
            std::string name;
            std::string col2;
            std::string col3;
            std::string col4;
        };

        GuideView currentView = GuideView::MENU;

        Button backButton;
        Button gameGuideButton;
        Button abilitiesGuideButton;
        Button offensiveTabButton;
        Button defensiveTabButton;

        TextObject titleText;
        TextObject subtitleText;
        TextObject hintText;

        UIManager::Panel mainPanel;
        UIManager::Panel contentPanel;

        AEGfxTexture* bgTexture = nullptr;
        AEGfxTexture* buttonTexture = nullptr;

        std::vector<AbilityRow> offensiveRows;
        std::vector<AbilityRow> defensiveRows;

        int offensiveScrollIndex = 0;
        int defensiveScrollIndex = 0;

        constexpr int VISIBLE_ABILITY_ROWS = 7;

        void OnOffensiveTabClicked();
        void OnDefensiveTabClicked();

        std::string FormatNumber(double value) {
            if (std::fabs(value - std::round(value)) < 0.001) {
                return std::to_string(static_cast<int>(std::round(value)));
            }

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << value;
            return oss.str();
        }

        std::string GetChargesText(const nlohmann::json& ability) {
            if (!ability.contains("Projectile_Stats")) {
                return "-";
            }

            const auto& proj = ability["Projectile_Stats"];
            if (proj.value("unlimited_charges", false)) {
                return "INF";
            }

            int charges = proj.value("Charges", 0);
            return (charges > 0) ? std::to_string(charges) : "-";
        }

        double GetBaseStatValue(const nlohmann::json& baseStats, const char* key1, const char* key2 = nullptr) {
            if (baseStats.contains(key1)) {
                return baseStats.value(key1, 0.0);
            }

            if (key2 != nullptr && baseStats.contains(key2)) {
                return baseStats.value(key2, 0.0);
            }

            return 0.0;
        }

        std::string TruncateText(const std::string& text, size_t maxLen) {
            if (text.size() <= maxLen) {
                return text;
            }

            if (maxLen <= 3) {
                return text.substr(0, maxLen);
            }

            return text.substr(0, maxLen - 3) + "...";
        }

        float PixelToNdcX(float pixelX) {
            return pixelX / (Config::SCREEN_WIDTH * 0.5f);
        }

        float PixelToNdcY(float pixelY) {
            return pixelY / (Config::SCREEN_HEIGHT * 0.5f);
        }

        void DrawTextLeftPx(const std::string& text, float pixelX, float pixelY, float scale, const Color& color) {
            AEGfxPrint(
                AssetsManager::textFont,
                text.c_str(),
                PixelToNdcX(pixelX),
                PixelToNdcY(pixelY),
                scale,
                color.Red(),
                color.Green(),
                color.Blue(),
                color.Alpha()
            );
        }

        AbilityRow BuildOffensiveRow(const nlohmann::json& ability) {
            AbilityRow row{};
            row.name = ability.value("Name", "Unknown");
            row.col2 = "-";
            row.col3 = "-";
            row.col4 = GetChargesText(ability);

            if (ability.contains("Projectile_Stats")) {
                const auto& proj = ability["Projectile_Stats"];
                row.col2 = FormatNumber(proj.value("Base_Damage", 0.0));
                row.col3 = FormatNumber(proj.value("Radius", 0.0));
            }

            return row;
        }

        AbilityRow BuildDefensiveRow(const nlohmann::json& ability) {
            AbilityRow row{};
            row.name = ability.value("Name", "Unknown");
            row.col2 = "Utility";
            row.col3 = "-";
            row.col4 = GetChargesText(ability);

            if (ability.contains("Defensive_Stats")) {
                const auto& defStats = ability["Defensive_Stats"];
                double shieldHp = defStats.value("ShieldHP", 0.0);

                if (shieldHp > 0.0) {
                    row.col2 = "Shield +" + FormatNumber(shieldHp);
                }
            }

            if (ability.contains("Base_Stats")) {
                const auto& baseStats = ability["Base_Stats"];

                double hp = GetBaseStatValue(baseStats, "HP", "Hp");
                double atk = GetBaseStatValue(baseStats, "ATK", "Attack");
                double shield = GetBaseStatValue(baseStats, "Shield");
                double rounds = GetBaseStatValue(baseStats, "Rounds");

                if (hp != 0.0) {
                    row.col2 = "HP +" + FormatNumber(hp);
                }
                else if (atk != 0.0) {
                    row.col2 = "ATK +" + FormatNumber(atk);
                }
                else if (shield != 0.0) {
                    row.col2 = "Shield +" + FormatNumber(shield);
                }

                if (rounds != 0.0) {
                    row.col3 = FormatNumber(rounds);
                }
            }

            return row;
        }

        std::vector<AbilityRow>& GetCurrentAbilityRows() {
            return (currentView == GuideView::ABILITIES_OFFENSIVE) ? offensiveRows : defensiveRows;
        }

        int& GetCurrentScrollIndex() {
            return (currentView == GuideView::ABILITIES_OFFENSIVE) ? offensiveScrollIndex : defensiveScrollIndex;
        }

        void ClampScrollIndex(int& scrollIndex, int rowCount) {
            int maxScroll = rowCount - VISIBLE_ABILITY_ROWS;
            if (maxScroll < 0) {
                maxScroll = 0;
            }

            if (scrollIndex < 0) {
                scrollIndex = 0;
            }
            if (scrollIndex > maxScroll) {
                scrollIndex = maxScroll;
            }
        }

        void ScrollAbilityTable(int delta) {
            std::vector<AbilityRow>& rows = GetCurrentAbilityRows();
            int& scrollIndex = GetCurrentScrollIndex();

            scrollIndex += delta;
            ClampScrollIndex(scrollIndex, static_cast<int>(rows.size()));
        }

        void DrawAbilityTable(
            const std::vector<AbilityRow>& rows,
            int scrollIndex,
            const std::string& col1Header,
            const std::string& col2Header,
            const std::string& col3Header,
            const std::string& col4Header) {

            // Table geometry in PIXELS relative to screen center
            const float tableCenterX = -8.0f;
            const float headerCenterY = 145.0f;

            const float tableWidth = 1120.0f;
            const float headerHeight = 46.0f;
            const float rowHeight = 48.0f;
            const float rowGap = 8.0f;
            const float rowStartCenterY = 92.0f;

            // Table background strip behind content
            UIManager::Panel tableBg(
                { tableWidth, 455.0f },
                Color(0.97f, 0.97f, 0.98f, 1.0f),
                Utils::AnchorMode::CENTER
            );
            tableBg.AddOffset({ tableCenterX, -8.0f });
            tableBg.Draw();

            // Header band
            UIManager::Panel headerBand(
                { tableWidth, headerHeight },
                Color(0.84f, 0.88f, 0.94f, 1.0f),
                Utils::AnchorMode::CENTER
            );
            headerBand.AddOffset({ tableCenterX, headerCenterY });
            headerBand.Draw();

            // Column text positions in PIXELS
            const float nameX = -540.0f;
            const float col2X = -120.0f;
            const float col3X = 120.0f;
            const float col4X = 330.0f;

            // Header text
            DrawTextLeftPx(col1Header, nameX, 135.0f, 0.62f, Color(0.05f, 0.07f, 0.10f, 1.0f));
            DrawTextLeftPx(col2Header, col2X, 135.0f, 0.62f, Color(0.05f, 0.07f, 0.10f, 1.0f));
            DrawTextLeftPx(col3Header, col3X, 135.0f, 0.62f, Color(0.05f, 0.07f, 0.10f, 1.0f));
            DrawTextLeftPx(col4Header, col4X, 135.0f, 0.62f, Color(0.05f, 0.07f, 0.10f, 1.0f));

            // Data rows
            for (int i = 0; i < VISIBLE_ABILITY_ROWS; ++i) {
                int rowIndex = scrollIndex + i;
                if (rowIndex >= static_cast<int>(rows.size())) {
                    break;
                }

                const AbilityRow& row = rows[rowIndex];
                const float rowCenterY = rowStartCenterY - (i * (rowHeight + rowGap));

                UIManager::Panel rowBand(
                    { tableWidth, rowHeight },
                    (i % 2 == 0)
                    ? Color(0.96f, 0.96f, 0.97f, 1.0f)
                    : Color(0.91f, 0.93f, 0.96f, 1.0f),
                    Utils::AnchorMode::CENTER
                );
                rowBand.AddOffset({ tableCenterX, rowCenterY });
                rowBand.Draw();

                const float textY = rowCenterY - 10.0f;

                DrawTextLeftPx(TruncateText(row.name, 22), nameX, textY, 0.54f, Color(0.08f, 0.08f, 0.08f, 1.0f));
                DrawTextLeftPx(TruncateText(row.col2, 14), col2X, textY, 0.54f, Color(0.08f, 0.08f, 0.08f, 1.0f));
                DrawTextLeftPx(TruncateText(row.col3, 12), col3X, textY, 0.54f, Color(0.08f, 0.08f, 0.08f, 1.0f));
                DrawTextLeftPx(TruncateText(row.col4, 10), col4X, textY, 0.54f, Color(0.08f, 0.08f, 0.08f, 1.0f));
            }

            // Draw scrollbar ONLY if scrolling is actually needed
            int rowCount = static_cast<int>(rows.size());
            if (rowCount > VISIBLE_ABILITY_ROWS) {
                const int maxScroll = rowCount - VISIBLE_ABILITY_ROWS;

                const float trackWidth = 16.0f;
                const float trackHeight = 392.0f;
                const float trackCenterX = 560.0f;
                const float trackCenterY = -12.0f;

                UIManager::Panel scrollTrack(
                    { trackWidth, trackHeight },
                    Color(0.82f, 0.86f, 0.92f, 1.0f),
                    Utils::AnchorMode::CENTER
                );
                scrollTrack.AddOffset({ trackCenterX, trackCenterY });
                scrollTrack.Draw();

                float thumbHeight = trackHeight * (static_cast<float>(VISIBLE_ABILITY_ROWS) / static_cast<float>(rowCount));
                if (thumbHeight < 60.0f) {
                    thumbHeight = 60.0f;
                }

                const float travel = trackHeight - thumbHeight;
                const float ratio = static_cast<float>(scrollIndex) / static_cast<float>(maxScroll);

                const float trackTop = trackCenterY + (trackHeight * 0.5f);
                const float thumbCenterY = trackTop - (thumbHeight * 0.5f) - (ratio * travel);

                UIManager::Panel scrollThumb(
                    { trackWidth, thumbHeight },
                    Color(0.33f, 0.53f, 0.88f, 1.0f),
                    Utils::AnchorMode::CENTER
                );
                scrollThumb.AddOffset({ trackCenterX, thumbCenterY });
                scrollThumb.Draw();
            }
        }

        std::vector<std::string> SplitLines(const std::string& text) {
            std::vector<std::string> lines;
            std::stringstream ss(text);
            std::string line;

            while (std::getline(ss, line)) {
                lines.push_back(line);
            }

            return lines;
        }

        void DrawCenteredMultilineText(
            const std::string& text,
            float scale,
            const Color& color,
            float centerX,
            float startY,
            float lineSpacing = 1.45f) {

            if (text.empty()) {
                return;
            }

            std::vector<std::string> lines = SplitLines(text);

            f32 charW = 0.0f;
            f32 charH = 0.0f;
            AEGfxGetPrintSize(AssetsManager::textFont, "A", scale, &charW, &charH);

            float currentY = startY - charH;

            for (const std::string& line : lines) {
                if (!line.empty()) {
                    f32 lineW = 0.0f;
                    f32 lineH = 0.0f;
                    AEGfxGetPrintSize(AssetsManager::textFont, line.c_str(), scale, &lineW, &lineH);

                    float drawX = centerX - (lineW * 0.5f);

                    AEGfxPrint(
                        AssetsManager::textFont,
                        line.c_str(),
                        drawX,
                        currentY,
                        scale,
                        color.Red(),
                        color.Green(),
                        color.Blue(),
                        color.Alpha()
                    );
                }

                currentY -= (charH * lineSpacing);
            }
        }

        std::string GetGameGuideText() {
            return
                "OBJECTIVE\n"
                "Destroy the enemy cannon before your own side collapses.\n\n"

                "FLOW OF A TURN\n"
                "1. Aim the cannon with the mouse.\n"
                "2. Hold Left Mouse Button to charge your shot.\n"
                "3. Release to fire.\n"
                "4. Let the projectile fully finish resolving.\n"
                "5. The turn then passes to the other side.\n"
                "6. Pick ability cards when they appear.\n\n"

                "ABILITY TYPES\n"
                "[OFF] Offensive shots that deal damage.\n"
                "[DEF] Defensive or utility tools.\n"
                "[STAT] Buffs or stat-changing effects.\n\n"

                "SMART TIPS\n"
                "Use terrain as cover instead of firing blindly.\n"
                "Large-radius attacks are strongest against clustered blocks.\n"
                "Protect your support structure so your cannon stays stable.\n"
                "Think about the enemy's next angle before ending your turn.\n\n"

                "QUICK CONTROLS\n"
                "Mouse  - Aim cannon\n"
                "Hold LMB  - Charge shot\n"
                "Release LMB  - Fire shot\n"
                "ESC  - Back / Pause\n"
                "I  - Instructions popup\n"
                "TAB  - Debug toggle";
        }

        void BuildAbilitiesPages() {
            offensiveRows.clear();
            defensiveRows.clear();
            offensiveScrollIndex = 0;
            defensiveScrollIndex = 0;

            std::ifstream file("Assets/DataAbilities/Abilities.json");
            if (!file.is_open()) {
                offensiveRows.push_back({ "Missing JSON file", "-", "-", "-" });
                defensiveRows.push_back({ "Missing JSON file", "-", "-", "-" });
                return;
            }

            nlohmann::json data;
            file >> data;

            for (const auto& ability : data) {
                const int type = ability.value("Type", 0);

                if (type == 2) {
                    offensiveRows.push_back(BuildOffensiveRow(ability));
                }
                else {
                    defensiveRows.push_back(BuildDefensiveRow(ability));
                }
            }

            if (offensiveRows.empty()) {
                offensiveRows.push_back({ "No offensive abilities", "-", "-", "-" });
            }

            if (defensiveRows.empty()) {
                defensiveRows.push_back({ "No defensive abilities", "-", "-", "-" });
            }
        }

        std::string GetTitleText() {
            switch (currentView) {
            case GuideView::MENU: return "GUIDE";
            case GuideView::GAME: return "GAME GUIDE";
            case GuideView::ABILITIES_OFFENSIVE:
            case GuideView::ABILITIES_DEFENSIVE:
                return "ABILITIES GUIDE";
            }
            return "GUIDE";
        }

        std::string GetSubtitleText() {
            switch (currentView) {
            case GuideView::MENU:
                return "Choose what you want to learn first";
            case GuideView::GAME:
                return "Core rules, controls, and practical tips";
            case GuideView::ABILITIES_OFFENSIVE:
                return "Page 1 / 2  -  Offensive abilities";
            case GuideView::ABILITIES_DEFENSIVE:
                return "Page 2 / 2  -  Defensive and utility abilities";
            }
            return "";
        }

        std::string GetHintText() {
            switch (currentView) {
            case GuideView::MENU:
                return "[1] Game Guide   |   [2] Abilities Guide   |   ESC / BACK - Main Menu";
            case GuideView::GAME:
                return "[BACK] Return to Guide Menu   |   [2] Open Abilities Guide";
            case GuideView::ABILITIES_OFFENSIVE:
            case GuideView::ABILITIES_DEFENSIVE:
                return "[1] Offensive   |   [2] Defensive   |   [W/S or Up/Down] Scroll   |   [BACK] Return to Guide Menu";
            }
            return "";
        }

        void OnOffensiveTabClicked() {
            currentView = GuideView::ABILITIES_OFFENSIVE;
            offensiveScrollIndex = 0;
        }

        void OnDefensiveTabClicked() {
            currentView = GuideView::ABILITIES_DEFENSIVE;
            defensiveScrollIndex = 0;
        }

        void RefreshTabColors() {
            const bool offensiveActive = (currentView == GuideView::ABILITIES_OFFENSIVE);
            const bool defensiveActive = (currentView == GuideView::ABILITIES_DEFENSIVE);

            offensiveTabButton.SetColor(
                offensiveActive
                ? Color(0.93f, 0.45f, 0.28f, 1.0f)
                : Color(0.85f, 0.78f, 0.72f, 1.0f)
            );

            defensiveTabButton.SetColor(
                defensiveActive
                ? Color(0.32f, 0.55f, 0.90f, 1.0f)
                : Color(0.76f, 0.82f, 0.92f, 1.0f)
            );
        }
    }

    void Load() {
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");
        buttonTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");
        BuildAbilitiesPages();
    }

    void Initialize() {
        currentView = GuideView::MENU;

        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        mainPanel = UIManager::Panel(
            { 1420.0f, 740.0f },
            Color(0.95f, 0.96f, 0.98f, 0.92f),
            Utils::AnchorMode::CENTER
        );

        contentPanel = UIManager::Panel(
            { 1240.0f, 500.0f },
            Color(1.0f, 1.0f, 1.0f, 0.97f),
            Utils::AnchorMode::CENTER
        );
        contentPanel.AddOffset({ 0.0f, 5.0f });

        backButton = Button(
            TextObject("< BACK"),
            Utils::AnchorMode::CENTER,
            { -680.0f, -325.0f },
            { 220.0f, 70.0f },
            Color(0.82f, 0.28f, 0.28f, 1.0f),
            buttonTexture,
            nullptr,
            OnBackClicked
        );

        gameGuideButton = Button(
            TextObject("GAME GUIDE"),
            Utils::AnchorMode::CENTER,
            { -300.0f, 70.0f },
            { 300.0f, 110.0f },
            Color(0.28f, 0.63f, 0.90f, 1.0f),
            buttonTexture,
            nullptr,
            OnGameGuideClicked
        );

        abilitiesGuideButton = Button(
            TextObject("ABILITIES GUIDE"),
            Utils::AnchorMode::CENTER,
            { 40.0f, 70.0f },
            { 300.0f, 110.0f },
            Color(0.56f, 0.44f, 0.88f, 1.0f),
            buttonTexture,
            nullptr,
            OnAbilitiesGuideClicked
        );

        offensiveTabButton = Button(
            TextObject("OFFENSIVE"),
            Utils::AnchorMode::CENTER,
            { -260.0f, 255.0f },
            { 250.0f, 65.0f },
            Color(0.93f, 0.45f, 0.28f, 1.0f),
            buttonTexture,
            nullptr,
            OnOffensiveTabClicked
        );

        defensiveTabButton = Button(
            TextObject("DEFENSIVE"),
            Utils::AnchorMode::CENTER,
            { 20.0f, 255.0f },
            { 250.0f, 65.0f },
            Color(0.76f, 0.82f, 0.92f, 1.0f),
            buttonTexture,
            nullptr,
            OnDefensiveTabClicked
        );

        titleText = TextObject(
            GetTitleText(),
            2.35f,
            Color(0.07f, 0.08f, 0.12f, 1.0f),
            { -0.10f, 0.76f }
        );

        subtitleText = TextObject(
            GetSubtitleText(),
            0.82f,
            Color(0.15f, 0.17f, 0.22f, 0.95f),
            { -0.20f, 0.63f }
        );

        hintText = TextObject(
            GetHintText(),
            0.58f,
            Color(0.05f, 0.05f, 0.05f, 0.95f),
            { -0.88f, -0.88f }
        );
    }

    void Draw() {
        if (bgTexture != nullptr) {
            GraphicsManager::DrawWorldUI(
                bgTexture,
                GraphicsManager::CreateTransformMatrix(
                    -Config::SCREEN_WIDTH / 2.0f,
                    Config::SCREEN_HEIGHT / 2.0f,
                    Config::SCREEN_WIDTH,
                    Config::SCREEN_HEIGHT
                ),
                Color(1.0f, 1.0f, 1.0f, 1.0f)
            );
        }
        else {
            AEGfxSetBackgroundColor(0.10f, 0.11f, 0.16f);
        }

        mainPanel.Draw();
        contentPanel.Draw();

        titleText.TextContent(GetTitleText());
        subtitleText.TextContent(GetSubtitleText());
        hintText.TextContent(GetHintText());

        titleText.Draw();
        subtitleText.Draw();
        hintText.Draw();

        if (currentView == GuideView::MENU) {
            TextObject menuBlurb(
                "Pick the kind of help you want.\n"
                "Game Guide explains the rules and controls.\n"
                "Abilities Guide explains what each ability group does.",
                0.72f,
                Color(0.12f, 0.12f, 0.12f, 1.0f),
                { -0.45f, 0.28f }
            );
            menuBlurb.Draw();
        }
        else if (currentView == GuideView::GAME) {
            DrawCenteredMultilineText(
                GetGameGuideText(),
                0.56f,
                Color(0.07f, 0.07f, 0.07f, 1.0f),
                0.0f,
                0.44f,
                1.40f
            );
        }
        else if (currentView == GuideView::ABILITIES_OFFENSIVE) {
            DrawAbilityTable(
                offensiveRows,
                offensiveScrollIndex,
                "NAME",
                "DAMAGE",
                "RADIUS",
                "CHARGES"
            );
        }
        else if (currentView == GuideView::ABILITIES_DEFENSIVE) {
            DrawAbilityTable(
                defensiveRows,
                defensiveScrollIndex,
                "NAME",
                "EFFECT",
                "DURATION",
                "CHARGES"
            );
        }
    }

    void Update() {
        if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_BACK)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnBackClicked();
            return;
        }

        if (currentView == GuideView::MENU) {
            if (AEInputCheckTriggered(AEVK_1)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnGameGuideClicked();
            }

            if (AEInputCheckTriggered(AEVK_2)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnAbilitiesGuideClicked();
            }
        }
        else if (currentView == GuideView::GAME) {
            if (AEInputCheckTriggered(AEVK_2)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnAbilitiesGuideClicked();
            }
        }
        else {
            if (AEInputCheckTriggered(AEVK_1)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnOffensiveTabClicked();
            }

            if (AEInputCheckTriggered(AEVK_2)) {
                AudioManager::PlaySFX(AudioManager::SFX::UIButton);
                OnDefensiveTabClicked();
            }

            if (AEInputCheckTriggered(AEVK_W) || AEInputCheckTriggered(AEVK_UP)) {
                ScrollAbilityTable(-1);
            }

            if (AEInputCheckTriggered(AEVK_S) || AEInputCheckTriggered(AEVK_DOWN)) {
                ScrollAbilityTable(+1);
            }
        }

        Draw();

        backButton.Update();

        if (currentView == GuideView::MENU) {
            gameGuideButton.Update();
            abilitiesGuideButton.Update();
        }
        else if (currentView == GuideView::ABILITIES_OFFENSIVE ||
            currentView == GuideView::ABILITIES_DEFENSIVE) {
            RefreshTabColors();
            offensiveTabButton.Update();
            defensiveTabButton.Update();
        }

        UICursor::Draw();
    }

    void Free() {}

    void Unload() {}

    void OnBackClicked() {
        if (currentView == GuideView::MENU) {
            GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
        }
        else {
            currentView = GuideView::MENU;
        }
    }

    void OnGameGuideClicked() {
        currentView = GuideView::GAME;
    }

    void OnAbilitiesGuideClicked() {
        currentView = GuideView::ABILITIES_OFFENSIVE;
        offensiveScrollIndex = 0;
        defensiveScrollIndex = 0;
    }
}