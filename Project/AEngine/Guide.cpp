/*
Authors: Bryan
Description: Display the game guide information

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "Guide.hpp"

#include "pch.hpp"
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

namespace Guide {

    void OnBackClicked();
    void OnGameGuideClicked();
    void OnAbilitiesGuideClicked();

    namespace {
        enum class GuideView {
            MENU,
            GAME,
            ABILITIES
        };

        enum class AbilityGroup {
            OFFENSIVE,
            DEFENSIVE
        };

        enum class AbilityRole {
            ATTACK,
            PROTECTION,
            UTILITY
        };

        struct AbilityGuideEntry {
            std::string name;
            std::string displayName;
            std::string description;
            AbilityGroup group = AbilityGroup::OFFENSIVE;
            AbilityRole role = AbilityRole::ATTACK;
            nlohmann::json raw;
            AEGfxTexture* imageTexture = nullptr;
        };

        struct AbilityDisplayRow {
            bool isSectionHeader = false;
            AbilityGroup group = AbilityGroup::OFFENSIVE;
            int entryIndex = -1;
            std::string label;
        };

        GuideView currentView = GuideView::MENU;

        Button backButton;
        Button gameGuideButton;
        Button abilitiesGuideButton;

        TextObject titleText;
        TextObject hintText;

        UIManager::Panel mainPanel;
        UIManager::Panel contentPanel;

        AEGfxTexture* bgTexture = nullptr;
        AEGfxTexture* buttonTexture = nullptr;

        std::vector<AbilityGuideEntry> abilityEntries;
        std::vector<AbilityDisplayRow> displayRows;

        int selectedEntryIndex = -1;
        int hoveredDisplayRowIndex = -1;
        int abilityScrollIndex = 0;

        constexpr int VISIBLE_ABILITY_ROWS = 8;
        constexpr float LIST_WIDTH = 620.0f;
        constexpr float DETAIL_WIDTH = 540.0f;
        constexpr float PANEL_HEIGHT = 500.0f;
        constexpr float LIST_CENTER_X = -380.0f;
        constexpr float DETAIL_CENTER_X = 355.0f;
        constexpr float SECTION_HEIGHT = 42.0f;
        constexpr float ROW_HEIGHT = 44.0f;
        constexpr float ROW_GAP = 6.0f;
        constexpr float FIRST_ROW_CENTER_Y = 165.0f;
        constexpr float LIST_HEADER_CENTER_Y = 215.0f;

        bool FileExists(const std::string& path) {
            std::ifstream file(path);
            return file.good();
        }

        std::string Trim(const std::string& text) {
            size_t start = 0;
            while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) {
                ++start;
            }

            size_t end = text.size();
            while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
                --end;
            }

            return text.substr(start, end - start);
        }

        std::string ToLower(std::string text) {
            for (char& c : text) {
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
            return text;
        }

        std::string FormatNumber(double value) {
            if (std::fabs(value - std::round(value)) < 0.001) {
                return std::to_string(static_cast<int>(std::round(value)));
            }

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << value;
            return oss.str();
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

        std::string GroupToString(AbilityGroup group) {
            switch (group) {
            case AbilityGroup::OFFENSIVE: return "OFFENSIVE";
            case AbilityGroup::DEFENSIVE: return "DEFENSIVE";
            }
            return "UNKNOWN";
        }

        std::string RoleToString(AbilityRole role) {
            switch (role) {
            case AbilityRole::ATTACK: return "Attack";
            case AbilityRole::PROTECTION: return "Protection";
            case AbilityRole::UTILITY: return "Utility";
            }
            return "Unknown";
        }

        Color GroupHeaderColor(AbilityGroup group) {
            switch (group) {
            case AbilityGroup::OFFENSIVE: return Color(0.90f, 0.48f, 0.30f, 1.0f);
            case AbilityGroup::DEFENSIVE: return Color(0.28f, 0.58f, 0.88f, 1.0f);
            }
            return Color(0.40f, 0.40f, 0.40f, 1.0f);
        }

        Color GroupMutedColor(AbilityGroup group) {
            switch (group) {
            case AbilityGroup::OFFENSIVE: return Color(0.95f, 0.88f, 0.84f, 1.0f);
            case AbilityGroup::DEFENSIVE: return Color(0.85f, 0.91f, 0.97f, 1.0f);
            }
            return Color(0.90f, 0.90f, 0.90f, 1.0f);
        }

        Color RoleColor(AbilityRole role) {
            switch (role) {
            case AbilityRole::ATTACK: return Color(0.85f, 0.28f, 0.20f, 1.0f);
            case AbilityRole::PROTECTION: return Color(0.20f, 0.56f, 0.90f, 1.0f);
            case AbilityRole::UTILITY: return Color(0.58f, 0.38f, 0.88f, 1.0f);
            }
            return Color(0.40f, 0.40f, 0.40f, 1.0f);
        }

        std::string BuildSafeAbilityToken(const std::string& name) {
            std::string token;
            token.reserve(name.size());

            for (char c : name) {
                if (std::isalnum(static_cast<unsigned char>(c))) {
                    token += c;
                }
                else if (c == ' ' || c == '-' || c == '/') {
                    token += '_';
                }
            }

            while (!token.empty() && token.back() == '_') {
                token.pop_back();
            }

            return token;
        }

        AbilityGroup DetermineGroup(const nlohmann::json& ability) {
            const std::string forcedGroup = ToLower(ability.value("Guide_Group", ""));
            if (forcedGroup == "offensive") {
                return AbilityGroup::OFFENSIVE;
            }
            if (forcedGroup == "defensive") {
                return AbilityGroup::DEFENSIVE;
            }

            return (ability.value("Type", 0) == 2)
                ? AbilityGroup::OFFENSIVE
                : AbilityGroup::DEFENSIVE;
        }

        AbilityRole DetermineRole(const nlohmann::json& ability, AbilityGroup group) {
            const std::string forcedRole = ToLower(ability.value("Guide_Role", ""));
            if (forcedRole == "attack") {
                return AbilityRole::ATTACK;
            }
            if (forcedRole == "protection" || forcedRole == "defense" || forcedRole == "defensive") {
                return AbilityRole::PROTECTION;
            }
            if (forcedRole == "utility") {
                return AbilityRole::UTILITY;
            }

            if (group == AbilityGroup::OFFENSIVE) {
                return AbilityRole::ATTACK;
            }

            if (ability.contains("Defensive_Stats") && !ability["Defensive_Stats"].is_null()) {
                const auto& defStats = ability["Defensive_Stats"];
                if (defStats.value("ShieldHP", 0.0) > 0.0) {
                    return AbilityRole::PROTECTION;
                }
            }

            if (ability.contains("Base_Stats") && !ability["Base_Stats"].is_null()) {
                const auto& baseStats = ability["Base_Stats"];
                const double hp = GetBaseStatValue(baseStats, "HP", "Hp");
                const double atk = GetBaseStatValue(baseStats, "ATK", "Attack");
                const double shield = GetBaseStatValue(baseStats, "Shield");
                if (hp > 0.0 || atk > 0.0 || shield > 0.0) {
                    return AbilityRole::PROTECTION;
                }
            }

            return AbilityRole::UTILITY;
        }

        AEGfxTexture* ResolveAbilityTexture(const nlohmann::json& ability) {
            std::vector<std::string> candidates;

            const std::string explicitGuideImage = ability.value("Guide_Image", "");
            if (!explicitGuideImage.empty()) {
                candidates.push_back(explicitGuideImage);
            }

            const std::string textureField = ability.value("Texture", "");
            if (!textureField.empty()) {
                if (textureField.find("Assets/") == 0) {
                    candidates.push_back(textureField);
                }
                else {
                    candidates.push_back("Assets/Abilities/" + textureField + ".png");
                }
            }

            const std::string token = BuildSafeAbilityToken(ability.value("Name", "Unknown"));
            if (!token.empty()) {
                candidates.push_back("Assets/Abilities/Card_" + token + ".png");
                candidates.push_back("Assets/Abilities/Icon_" + token + ".png");
            }

            candidates.push_back("Assets/Abilities/Card_Blank.png");
            candidates.push_back("Assets/Abilities/Icon_Blank.png");
            candidates.push_back("Assets/square.png");

            for (const std::string& candidate : candidates) {
                if (!FileExists(candidate)) {
                    continue;
                }

                AEGfxTexture* texture = AssetsManager::GetTexture(candidate.c_str());
                if (texture != nullptr) {
                    return texture;
                }
            }

            return AssetsManager::squareTexture;
        }

        bool ShouldIncludeGuideEntry(const nlohmann::json& ability) {
            if (!ability.is_object()) {
                return false;
            }

            if (ability.value("Guide_Hidden", false)) {
                return false;
            }

            const std::string name = Trim(ability.value("Name", ""));
            if (name.empty()) {
                return false;
            }

            return true;
        }

        struct NameCountEntry {
            std::string loweredName;
            int count = 0;
        };

        int FindNameCountIndex(const std::vector<NameCountEntry>& nameCounts, const std::string& loweredName) {
            for (int i = 0; i < static_cast<int>(nameCounts.size()); ++i) {
                if (nameCounts[i].loweredName == loweredName) {
                    return i;
                }
            }
            return -1;
        }

        void BuildDisplayNamesForDuplicates() {
            std::vector<NameCountEntry> nameCounts;

            for (const AbilityGuideEntry& entry : abilityEntries) {
                const std::string loweredName = ToLower(entry.name);
                const int existingIndex = FindNameCountIndex(nameCounts, loweredName);

                if (existingIndex >= 0) {
                    ++nameCounts[existingIndex].count;
                }
                else {
                    NameCountEntry countEntry{};
                    countEntry.loweredName = loweredName;
                    countEntry.count = 1;
                    nameCounts.push_back(countEntry);
                }
            }

            for (AbilityGuideEntry& entry : abilityEntries) {
                entry.displayName = entry.name;

                const int existingIndex = FindNameCountIndex(nameCounts, ToLower(entry.name));
                if (existingIndex < 0 || nameCounts[existingIndex].count <= 1) {
                    continue;
                }

                const int tier = entry.raw.value("Tier", 0);
                if (tier > 0) {
                    entry.displayName += " (Tier " + std::to_string(tier) + ")";
                }
                else {
                    entry.displayName += " (Variant)";
                }
            }
        }

        int GroupOrder(AbilityGroup group) {
            switch (group) {
            case AbilityGroup::OFFENSIVE: return 0;
            case AbilityGroup::DEFENSIVE: return 1;
            }
            return 99;
        }

        void BuildDisplayRows() {
            displayRows.clear();

            const AbilityGroup orderedGroups[] = {
                AbilityGroup::OFFENSIVE,
                AbilityGroup::DEFENSIVE
            };

            for (AbilityGroup group : orderedGroups) {
                bool hasAny = false;
                for (int i = 0; i < static_cast<int>(abilityEntries.size()); ++i) {
                    if (abilityEntries[i].group == group) {
                        if (!hasAny) {
                            displayRows.push_back({ true, group, -1, GroupToString(group) });
                            hasAny = true;
                        }
                        displayRows.push_back({ false, group, i, abilityEntries[i].displayName });
                    }
                }
            }
        }

        void ClampAbilityScrollIndex() {
            int maxScroll = static_cast<int>(displayRows.size()) - VISIBLE_ABILITY_ROWS;
            if (maxScroll < 0) {
                maxScroll = 0;
            }
            abilityScrollIndex = std::clamp(abilityScrollIndex, 0, maxScroll);
        }

        void ScrollAbilityList(int delta) {
            abilityScrollIndex += delta;
            ClampAbilityScrollIndex();
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
                color.Alpha());
        }

        void DrawTextCenteredPx(const std::string& text, float centerPixelX, float pixelY, float scale, const Color& color) {
            f32 textW = 0.0f;
            f32 textH = 0.0f;
            AEGfxGetPrintSize(AssetsManager::textFont, text.c_str(), scale, &textW, &textH);

            AEGfxPrint(
                AssetsManager::textFont,
                text.c_str(),
                PixelToNdcX(centerPixelX) - (textW * 0.5f),
                PixelToNdcY(pixelY),
                scale,
                color.Red(),
                color.Green(),
                color.Blue(),
                color.Alpha());
        }

        std::vector<std::string> WrapParagraph(const std::string& paragraph, size_t maxCharsPerLine) {
            std::vector<std::string> lines;
            std::istringstream words(paragraph);
            std::string word;
            std::string currentLine;

            while (words >> word) {
                if (currentLine.empty()) {
                    currentLine = word;
                }
                else if (currentLine.size() + 1 + word.size() <= maxCharsPerLine) {
                    currentLine += ' ';
                    currentLine += word;
                }
                else {
                    lines.push_back(currentLine);
                    currentLine = word;
                }
            }

            if (!currentLine.empty()) {
                lines.push_back(currentLine);
            }

            if (lines.empty()) {
                lines.push_back("");
            }

            return lines;
        }

        std::vector<std::string> WrapText(const std::string& text, size_t maxCharsPerLine) {
            std::vector<std::string> result;
            std::stringstream ss(text);
            std::string paragraph;

            while (std::getline(ss, paragraph)) {
                if (paragraph.empty()) {
                    result.push_back("");
                    continue;
                }

                std::vector<std::string> wrapped = WrapParagraph(paragraph, maxCharsPerLine);
                result.insert(result.end(), wrapped.begin(), wrapped.end());
            }

            return result;
        }

        void DrawWrappedTextLeftPxLimited(
            const std::string& text,
            float pixelX,
            float pixelY,
            float scale,
            const Color& color,
            size_t maxCharsPerLine,
            float lineStep,
            int maxLines)
        {
            std::vector<std::string> lines = WrapText(text, maxCharsPerLine);

            float y = pixelY;
            int linesToDraw = std::min<int>(static_cast<int>(lines.size()), maxLines);

            for (int i = 0; i < linesToDraw; ++i) {
                DrawTextLeftPx(lines[i], pixelX, y, scale, color);
                y -= lineStep;
            }

            if (static_cast<int>(lines.size()) > maxLines) {
                DrawTextLeftPx("...", pixelX, y, scale, color);
            }
        }

        std::string BuildAbilityDescription(const AbilityGuideEntry& entry) {
            std::string description = Trim(entry.description);
            const auto& ability = entry.raw;

            if (!description.empty()) {
                return description;
            }

            std::ostringstream oss;

            if (entry.role == AbilityRole::UTILITY) {
                oss << "Utility ability";
            }
            else if (entry.role == AbilityRole::PROTECTION) {
                oss << "Defensive / protection ability";
            }
            else {
                oss << "Offensive attack ability";
            }

            if (ability.contains("Projectile_Stats") && !ability["Projectile_Stats"].is_null()) {
                const auto& proj = ability["Projectile_Stats"];
                const double damage = proj.value("Base_Damage", 0.0);
                const double radius = proj.value("Radius", 0.0);

                if (damage > 0.0 || radius > 0.0) {
                    oss << " with";

                    bool wroteAny = false;

                    if (damage > 0.0) {
                        oss << " damage " << FormatNumber(damage);
                        wroteAny = true;
                    }

                    if (radius > 0.0) {
                        if (wroteAny) {
                            oss << " and";
                        }
                        oss << " radius " << FormatNumber(radius);
                    }
                }
            }

            if (ability.contains("Status_Stats") && !ability["Status_Stats"].is_null()) {
                const auto& status = ability["Status_Stats"];
                const int rounds = status.value("Rounds", 0);
                if (rounds > 0) {
                    oss << ". Lasts " << rounds << " round(s)";
                }
            }

            oss << ".";

            return oss.str();
        }

        std::vector<std::string> BuildStatLines(const AbilityGuideEntry& entry) {
            std::vector<std::string> lines;
            const auto& ability = entry.raw;

            lines.push_back("Tier: " + std::to_string(ability.value("Tier", 0)));

            bool durationAlreadyShown = false;

            if (entry.role == AbilityRole::ATTACK) {
                if (ability.contains("Projectile_Stats") && !ability["Projectile_Stats"].is_null()) {
                    const auto& proj = ability["Projectile_Stats"];

                    const double damage = proj.value("Base_Damage", 0.0);
                    const double radius = proj.value("Radius", 0.0);
                    const double amp = proj.value("Damage_Amp", 0.0);

                    if (damage > 0.0) {
                        lines.push_back("Damage: " + FormatNumber(damage));
                    }
                    if (radius > 0.0) {
                        lines.push_back("Radius: " + FormatNumber(radius));
                    }
                    if (amp > 0.0) {
                        lines.push_back("Damage Amp: " + FormatNumber(amp));
                    }

                    const bool unlimitedCharges = proj.value("unlimited_charges", false);
                    const int charges = proj.value("Charges", 0);

                    if (unlimitedCharges) {
                        lines.push_back("Charges: Unlimited");
                    }
                    else if (charges > 0) {
                        lines.push_back("Charges: " + std::to_string(charges));
                    }
                }
            }
            else if (entry.role == AbilityRole::PROTECTION) {
                if (ability.contains("Defensive_Stats") && !ability["Defensive_Stats"].is_null()) {
                    const auto& defStats = ability["Defensive_Stats"];
                    const double shieldHp = defStats.value("ShieldHP", 0.0);
                    if (shieldHp > 0.0) {
                        lines.push_back("Shield HP: " + FormatNumber(shieldHp));
                    }
                }

                if (ability.contains("Base_Stats") && !ability["Base_Stats"].is_null()) {
                    const auto& baseStats = ability["Base_Stats"];

                    const double hp = GetBaseStatValue(baseStats, "HP", "Hp");
                    const double atk = GetBaseStatValue(baseStats, "ATK", "Attack");
                    const double shield = GetBaseStatValue(baseStats, "Shield");

                    if (hp > 0.0) {
                        lines.push_back("HP: " + FormatNumber(hp));
                    }
                    if (atk > 0.0) {
                        lines.push_back("ATK: " + FormatNumber(atk));
                    }
                    if (shield > 0.0) {
                        lines.push_back("Shield: " + FormatNumber(shield));
                    }
                }
            }
            else {
                if (ability.contains("Status_Stats") && !ability["Status_Stats"].is_null()) {
                    const auto& status = ability["Status_Stats"];

                    const int rounds = status.value("Rounds", 0);
                    const double gravityScale = status.value("Gravity_Scale", 0.0);
                    const double damageEachRound = status.value("Damage_Each_Round", 0.0);
                    const double effectRadius = status.value("Effect_Radius", 0.0);

                    if (rounds > 0) {
                        lines.push_back("Duration: " + std::to_string(rounds) + " round(s)");
                        durationAlreadyShown = true;
                    }
                    if (gravityScale > 0.0) {
                        lines.push_back("Gravity Scale: " + FormatNumber(gravityScale));
                    }
                    if (damageEachRound > 0.0) {
                        lines.push_back("Damage / Round: " + FormatNumber(damageEachRound));
                    }
                    if (effectRadius > 0.0) {
                        lines.push_back("Effect Radius: " + FormatNumber(effectRadius));
                    }
                }
            }

            if (!durationAlreadyShown &&
                ability.contains("Status_Stats") &&
                !ability["Status_Stats"].is_null()) {
                const auto& status = ability["Status_Stats"];
                const int rounds = status.value("Rounds", 0);
                if (rounds > 0) {
                    lines.push_back("Duration: " + std::to_string(rounds) + " round(s)");
                }
            }

            if (lines.size() == 1) {
                lines.push_back("No major stat data");
            }

            return lines;
        }

        bool CompareAbilityEntries(const AbilityGuideEntry& a, const AbilityGuideEntry& b) {
            const int groupA = GroupOrder(a.group);
            const int groupB = GroupOrder(b.group);
            if (groupA != groupB) {
                return groupA < groupB;
            }

            const std::string nameA = ToLower(a.name);
            const std::string nameB = ToLower(b.name);

            if (nameA != nameB) {
                return nameA < nameB;
            }

            const int tierA = a.raw.value("Tier", 0);
            const int tierB = b.raw.value("Tier", 0);

            if (tierA != tierB) {
                return tierA < tierB;
            }

            return ToLower(a.displayName) < ToLower(b.displayName);
        }

        void LoadAbilityJsonFile(const char* path) {
            std::ifstream file(path);
            if (!file.is_open()) {
                return;
            }

            nlohmann::json data;
            file >> data;

            for (const auto& ability : data) {
                if (!ShouldIncludeGuideEntry(ability)) {
                    continue;
                }

                AbilityGuideEntry entry{};
                entry.name = Trim(ability.value("Name", "Unknown Ability"));
                entry.displayName = entry.name;
                entry.description = ability.value("Description", "");
                entry.group = DetermineGroup(ability);
                entry.role = DetermineRole(ability, entry.group);
                entry.raw = ability;
                entry.imageTexture = ResolveAbilityTexture(ability);
                abilityEntries.push_back(entry);
            }
        }

        void BuildAbilitiesPages() {
            abilityEntries.clear();
            displayRows.clear();
            abilityScrollIndex = 0;
            hoveredDisplayRowIndex = -1;
            selectedEntryIndex = -1;

            // Keep the player-facing guide synced with the actual gameplay pool.
            // PlayerAbilities.json and AiAbilities.json currently contain entries
            // that are broader than the live gameplay ability list.
            LoadAbilityJsonFile("Assets/DataAbilities/Abilities.json");

            std::sort(abilityEntries.begin(), abilityEntries.end(), CompareAbilityEntries);
            BuildDisplayNamesForDuplicates();
            BuildDisplayRows();

            for (const AbilityDisplayRow& row : displayRows) {
                if (!row.isSectionHeader && row.entryIndex >= 0) {
                    selectedEntryIndex = row.entryIndex;
                    break;
                }
            }
        }

        std::string GetTitleText() {
            switch (currentView) {
            case GuideView::MENU:      return "GUIDE";
            case GuideView::GAME:      return "GAME GUIDE";
            case GuideView::ABILITIES: return "ABILITIES GUIDE";
            }
            return "GUIDE";
        }

        std::string GetSubtitleText() {
            switch (currentView) {
            case GuideView::MENU:
                return "Choose what you want to learn first";
            case GuideView::GAME:
                return "Core rules, controls, and practical tips";
            case GuideView::ABILITIES:
                return "Offensive and Defensive abilities. Utility is shown under Role.";
            }
            return "";
        }

        std::string GetHintText() {
            switch (currentView) {
            case GuideView::MENU:
                return "[1] Game Guide   |   [2] Abilities Guide   |   ESC / BACK - Main Menu";
            case GuideView::GAME:
                return "[BACK] Return to Guide Menu   |   [2] Open Abilities Guide";
            case GuideView::ABILITIES:
                return "Click an ability row to inspect   |   [W/S or Up/Down or Mouse Wheel] Scroll   |   [BACK] Return to Guide Menu";
            }
            return "";
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
                "ABILITY GROUPS\n"
                "[OFFENSIVE] Attack-focused and damage-focused abilities.\n"
                "[DEFENSIVE] Protective, support, and control abilities.\n"
                "[UTILITY] Utility is shown as a ROLE inside the abilities table and detail panel.\n\n"
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

        bool IsPointInsideCenteredRect(float pointX, float pointY, float rectCenterX, float rectCenterY, float rectWidth, float rectHeight) {
            const float halfW = rectWidth * 0.5f;
            const float halfH = rectHeight * 0.5f;

            return (pointX >= rectCenterX - halfW && pointX <= rectCenterX + halfW &&
                pointY >= rectCenterY - halfH && pointY <= rectCenterY + halfH);
        }

        void UpdateHoveredRow() {
            hoveredDisplayRowIndex = -1;

            float mouseX = 0.0f;
            float mouseY = 0.0f;
            Utils::GetWorldMousePos(mouseX, mouseY);

            for (int i = 0; i < VISIBLE_ABILITY_ROWS; ++i) {
                const int displayIndex = abilityScrollIndex + i;
                if (displayIndex >= static_cast<int>(displayRows.size())) {
                    break;
                }

                const AbilityDisplayRow& row = displayRows[displayIndex];
                const float rowBoxHeight = row.isSectionHeader ? SECTION_HEIGHT : ROW_HEIGHT;
                const float rowCenterY = FIRST_ROW_CENTER_Y - (i * (ROW_HEIGHT + ROW_GAP));

                if (IsPointInsideCenteredRect(mouseX, mouseY, LIST_CENTER_X, rowCenterY, LIST_WIDTH, rowBoxHeight)) {
                    hoveredDisplayRowIndex = displayIndex;
                    return;
                }
            }
        }

        void HandleAbilityRowClick() {
            if (hoveredDisplayRowIndex < 0 || hoveredDisplayRowIndex >= static_cast<int>(displayRows.size())) {
                return;
            }

            const AbilityDisplayRow& row = displayRows[hoveredDisplayRowIndex];
            if (row.isSectionHeader || row.entryIndex < 0) {
                return;
            }

            selectedEntryIndex = row.entryIndex;
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
        }

        void DrawCenteredMultilineText(const std::string& text, float scale, const Color& color,
            float centerX, float startY, float lineSpacing = 1.45f) {

            if (text.empty()) {
                return;
            }

            std::stringstream ss(text);
            std::vector<std::string> lines;
            std::string line;
            while (std::getline(ss, line)) {
                lines.push_back(line);
            }

            f32 charW = 0.0f;
            f32 charH = 0.0f;
            AEGfxGetPrintSize(AssetsManager::textFont, "A", scale, &charW, &charH);

            float currentY = startY - charH;
            for (const std::string& lineText : lines) {
                if (!lineText.empty()) {
                    f32 lineW = 0.0f;
                    f32 lineH = 0.0f;
                    AEGfxGetPrintSize(AssetsManager::textFont, lineText.c_str(), scale, &lineW, &lineH);
                    float drawX = centerX - (lineW * 0.5f);
                    AEGfxPrint(
                        AssetsManager::textFont,
                        lineText.c_str(),
                        drawX,
                        currentY,
                        scale,
                        color.Red(),
                        color.Green(),
                        color.Blue(),
                        color.Alpha());
                }
                currentY -= (charH * lineSpacing);
            }
        }

        void DrawAbilityList() {
            UIManager::Panel listPanel(
                { LIST_WIDTH, PANEL_HEIGHT },
                Color(0.98f, 0.985f, 0.99f, 1.0f),
                Utils::AnchorMode::CENTER);
            listPanel.AddOffsetPosition({ LIST_CENTER_X, 10.0f });
            listPanel.Draw();

            UIManager::Panel headerBand(
                { LIST_WIDTH, 48.0f },
                Color(0.84f, 0.88f, 0.94f, 1.0f),
                Utils::AnchorMode::CENTER);
            headerBand.AddOffsetPosition({ LIST_CENTER_X, LIST_HEADER_CENTER_Y });
            headerBand.Draw();

            DrawTextCenteredPx(
                "ABILITY",
                LIST_CENTER_X,
                LIST_HEADER_CENTER_Y - 10.0f,
                0.62f,
                Color(0.05f, 0.07f, 0.10f, 1.0f));

            for (int i = 0; i < VISIBLE_ABILITY_ROWS; ++i) {
                const int displayIndex = abilityScrollIndex + i;
                if (displayIndex >= static_cast<int>(displayRows.size())) {
                    break;
                }

                const AbilityDisplayRow& row = displayRows[displayIndex];
                const float rowCenterY = FIRST_ROW_CENTER_Y - (i * (ROW_HEIGHT + ROW_GAP));

                if (row.isSectionHeader) {
                    UIManager::Panel sectionBand(
                        { LIST_WIDTH, SECTION_HEIGHT },
                        GroupHeaderColor(row.group),
                        Utils::AnchorMode::CENTER);
                    sectionBand.AddOffsetPosition({ LIST_CENTER_X, rowCenterY });
                    sectionBand.Draw();

                    const int count = static_cast<int>(std::count_if(
                        abilityEntries.begin(),
                        abilityEntries.end(),
                        [row](const AbilityGuideEntry& entry) {
                            return entry.group == row.group;
                        }));

                    std::string sectionText = row.label + "  (" + std::to_string(count) + " abilities)";
                    DrawTextCenteredPx(
                        sectionText,
                        LIST_CENTER_X,
                        rowCenterY - 9.0f,
                        0.58f,
                        Color(1.0f, 1.0f, 1.0f, 1.0f));
                    continue;
                }

                const AbilityGuideEntry& entry = abilityEntries[row.entryIndex];
                const bool isSelected = (row.entryIndex == selectedEntryIndex);
                const bool isHovered = (displayIndex == hoveredDisplayRowIndex);

                Color rowColor = (i % 2 == 0)
                    ? Color(0.96f, 0.96f, 0.97f, 1.0f)
                    : Color(0.91f, 0.93f, 0.96f, 1.0f);

                if (isHovered) {
                    rowColor = GroupMutedColor(entry.group);
                }
                if (isSelected) {
                    rowColor = Color(0.78f, 0.87f, 0.98f, 1.0f);
                }

                UIManager::Panel rowBand(
                    { LIST_WIDTH, ROW_HEIGHT },
                    rowColor,
                    Utils::AnchorMode::CENTER);
                rowBand.AddOffsetPosition({ LIST_CENTER_X, rowCenterY });
                rowBand.Draw();

                DrawTextCenteredPx(
                    entry.displayName,
                    LIST_CENTER_X,
                    rowCenterY - 10.0f,
                    0.52f,
                    Color(0.08f, 0.08f, 0.08f, 1.0f));
            }

            if (static_cast<int>(displayRows.size()) > VISIBLE_ABILITY_ROWS) {
                const int maxScroll = static_cast<int>(displayRows.size()) - VISIBLE_ABILITY_ROWS;
                const float trackWidth = 16.0f;
                const float trackHeight = 406.0f;
                const float trackCenterX = LIST_CENTER_X + 330.0f;
                const float trackCenterY = -10.0f;

                UIManager::Panel scrollTrack(
                    { trackWidth, trackHeight },
                    Color(0.82f, 0.86f, 0.92f, 1.0f),
                    Utils::AnchorMode::CENTER);
                scrollTrack.AddOffsetPosition({ trackCenterX, trackCenterY });
                scrollTrack.Draw();

                float thumbHeight = trackHeight *
                    (static_cast<float>(VISIBLE_ABILITY_ROWS) / static_cast<float>(displayRows.size()));
                if (thumbHeight < 60.0f) {
                    thumbHeight = 60.0f;
                }

                const float travel = trackHeight - thumbHeight;
                const float ratio = (maxScroll > 0)
                    ? static_cast<float>(abilityScrollIndex) / static_cast<float>(maxScroll)
                    : 0.0f;

                const float trackTop = trackCenterY + (trackHeight * 0.5f);
                const float thumbCenterY = trackTop - (thumbHeight * 0.5f) - (ratio * travel);

                UIManager::Panel scrollThumb(
                    { trackWidth, thumbHeight },
                    Color(0.33f, 0.53f, 0.88f, 1.0f),
                    Utils::AnchorMode::CENTER);
                scrollThumb.AddOffsetPosition({ trackCenterX, thumbCenterY });
                scrollThumb.Draw();
            }
        }

        void DrawAbilityDetail() {
            UIManager::Panel detailPanel(
                { DETAIL_WIDTH, PANEL_HEIGHT },
                Color(0.98f, 0.985f, 0.99f, 1.0f),
                Utils::AnchorMode::CENTER);
            detailPanel.AddOffsetPosition({ DETAIL_CENTER_X, 10.0f });
            detailPanel.Draw();

            if (selectedEntryIndex < 0 || selectedEntryIndex >= static_cast<int>(abilityEntries.size())) {
                DrawTextLeftPx(
                    "No ability selected.",
                    DETAIL_CENTER_X - 225.0f,
                    180.0f,
                    0.62f,
                    Color(0.10f, 0.10f, 0.10f, 1.0f));
                return;
            }

            const AbilityGuideEntry& entry = abilityEntries[selectedEntryIndex];
            const float leftX = DETAIL_CENTER_X - 225.0f;

            UIManager::Panel imagePanel(
                { 118.0f, 118.0f },
                GroupMutedColor(entry.group),
                Utils::AnchorMode::CENTER);
            imagePanel.AddOffsetPosition({ DETAIL_CENTER_X - 175.0f, 172.0f });
            imagePanel.Draw();

            if (entry.imageTexture != nullptr) {
                GraphicsManager::DrawScreenOverlay(
                    entry.imageTexture,
                    GraphicsManager::CreateTransformMatrix(
                        DETAIL_CENTER_X - 234.0f,
                        231.0f,
                        118.0f,
                        118.0f),
                    Color(1.0f, 1.0f, 1.0f, 1.0f));
            }

            DrawTextLeftPx(entry.displayName, DETAIL_CENTER_X - 35.0f, 214.0f, 0.62f, Color(0.05f, 0.07f, 0.10f, 1.0f));
            DrawTextLeftPx(GroupToString(entry.group), DETAIL_CENTER_X - 35.0f, 187.0f, 0.46f, GroupHeaderColor(entry.group));
            DrawTextLeftPx("Role: " + RoleToString(entry.role), DETAIL_CENTER_X - 35.0f, 164.0f, 0.44f, RoleColor(entry.role));

            const float descriptionBoxHeight = 132.0f;
            const float descriptionBoxCenterY = 8.0f;
            const float statsBoxHeight = 122.0f;
            const float statsBoxCenterY = -170.0f;

            UIManager::Panel descriptionBox(
                { DETAIL_WIDTH - 34.0f, descriptionBoxHeight },
                Color(0.94f, 0.95f, 0.97f, 1.0f),
                Utils::AnchorMode::CENTER);
            descriptionBox.AddOffsetPosition({ DETAIL_CENTER_X, descriptionBoxCenterY });
            descriptionBox.Draw();

            DrawTextLeftPx("DESCRIPTION", leftX, 72.0f, 0.50f, Color(0.05f, 0.07f, 0.10f, 1.0f));

            DrawWrappedTextLeftPxLimited(
                BuildAbilityDescription(entry),
                leftX,
                44.0f,
                0.38f,
                Color(0.12f, 0.12f, 0.12f, 1.0f),
                55,
                18.0f,
                4
            );

            DrawTextLeftPx("KEY STATS", leftX, -92.0f, 0.50f, Color(0.05f, 0.07f, 0.10f, 1.0f));

            UIManager::Panel statsBox(
                { DETAIL_WIDTH - 34.0f, statsBoxHeight },
                Color(0.94f, 0.95f, 0.97f, 1.0f),
                Utils::AnchorMode::CENTER);
            statsBox.AddOffsetPosition({ DETAIL_CENTER_X, statsBoxCenterY });
            statsBox.Draw();

            std::vector<std::string> statLines = BuildStatLines(entry);

            const float column1X = leftX;
            const float column2X = leftX + 220.0f;
            float y1 = -122.0f;
            float y2 = -122.0f;

            int maxTotalLines = 6;
            int visibleLines = std::min<int>(static_cast<int>(statLines.size()), maxTotalLines);

            for (int i = 0; i < visibleLines; ++i) {
                const std::string text = "- " + statLines[i];

                if (i < 3) {
                    DrawTextLeftPx(text, column1X, y1, 0.39f, Color(0.12f, 0.12f, 0.12f, 1.0f));
                    y1 -= 19.0f;
                }
                else {
                    DrawTextLeftPx(text, column2X, y2, 0.39f, Color(0.12f, 0.12f, 0.12f, 1.0f));
                    y2 -= 19.0f;
                }
            }

            if (static_cast<int>(statLines.size()) > maxTotalLines) {
                DrawTextLeftPx("...", column2X, y2, 0.39f, Color(0.12f, 0.12f, 0.12f, 1.0f));
            }
        }
    }

    void Load() {
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");
        buttonTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");
        BuildAbilitiesPages();
    }

    void Initialize() {
        currentView = GuideView::MENU;
        hoveredDisplayRowIndex = -1;

        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        mainPanel = UIManager::Panel(
            { 1480.0f, 760.0f },
            Color(0.95f, 0.96f, 0.98f, 0.92f),
            Utils::AnchorMode::CENTER);

        contentPanel = UIManager::Panel(
            { 1320.0f, 520.0f },
            Color(1.0f, 1.0f, 1.0f, 0.97f),
            Utils::AnchorMode::CENTER);
        contentPanel.AddOffsetPosition({ 0.0f, 5.0f });

        backButton = Button(
            TextObject("< BACK"),
            Utils::AnchorMode::CENTER,
            { -680.0f, -300.0f },
            { 220.0f, 70.0f },
            Color(0.82f, 0.28f, 0.28f, 1.0f),
            buttonTexture,
            nullptr,
            OnBackClicked);

        const float menuButtonWidth = 300.0f;
        const float menuButtonHeight = 110.0f;
        const float menuButtonGap = 70.0f;

        // These are the visual centers we actually want on screen.
        const float visualGroupCenterX = 0.0f;
        const float visualButtonCenterY = 25.0f;

        const float leftVisualCenterX =
            visualGroupCenterX - (menuButtonWidth * 0.5f) - (menuButtonGap * 0.5f);
        const float rightVisualCenterX =
            visualGroupCenterX + (menuButtonWidth * 0.5f) + (menuButtonGap * 0.5f);

        // Button placement appears to behave like top-left style placement internally,
        // even when using AnchorMode::CENTER, so compensate by subtracting half-size.
        const float buttonPosY = visualButtonCenterY - (menuButtonHeight * 0.5f);
        const float leftButtonPosX = leftVisualCenterX - (menuButtonWidth * 0.5f);
        const float rightButtonPosX = rightVisualCenterX - (menuButtonWidth * 0.5f);

        gameGuideButton = Button(
            TextObject("GAME GUIDE"),
            Utils::AnchorMode::CENTER,
            { leftButtonPosX, buttonPosY },
            { menuButtonWidth, menuButtonHeight },
            Color(0.28f, 0.63f, 0.90f, 1.0f),
            buttonTexture,
            nullptr,
            OnGameGuideClicked);

        abilitiesGuideButton = Button(
            TextObject("ABILITIES GUIDE"),
            Utils::AnchorMode::CENTER,
            { rightButtonPosX, buttonPosY },
            { menuButtonWidth, menuButtonHeight },
            Color(0.56f, 0.44f, 0.88f, 1.0f),
            buttonTexture,
            nullptr,
            OnAbilitiesGuideClicked);

        titleText = TextObject(
            GetTitleText(),
            1.0f,
            Color(0.07f, 0.08f, 0.12f, 1.0f));
        titleText.SetPosition(Utils::AnchorMode::TOP_CENTER, mainPanel.GetTransform());
        titleText.SetOffsetPosition(0, -10.0f);

        hintText = TextObject(
            GetHintText(),
            0.5f,
            Color(0.05f, 0.05f, 0.05f, 0.92f),
            { -0.93f, -0.88f });
    }

    void Draw() {
        if (bgTexture != nullptr) {
            GraphicsManager::DrawWorldUI(
                bgTexture,
                GraphicsManager::CreateTransformMatrix(
                    -Config::SCREEN_WIDTH / 2.0f,
                    Config::SCREEN_HEIGHT / 2.0f,
                    Config::SCREEN_WIDTH,
                    Config::SCREEN_HEIGHT),
                Color(1.0f, 1.0f, 1.0f, 1.0f));
        }
        else {
            AEGfxSetBackgroundColor(0.10f, 0.11f, 0.16f);
        }

        mainPanel.Draw();
        contentPanel.Draw();

        titleText.TextContent(GetTitleText());
        titleText.Draw();

        hintText.TextContent(GetHintText());
        hintText.Draw();

        if (currentView == GuideView::MENU) {
            DrawCenteredMultilineText(
                "Pick the kind of help you want.\n"
                "Game Guide explains the rules and controls.\n"
                "Abilities Guide shows the real gameplay ability pool.",
                0.72f,
                Color(0.12f, 0.12f, 0.12f, 1.0f),
                0.0f,
                0.33f,
                1.25f);
        }
        else if (currentView == GuideView::GAME) {
            DrawCenteredMultilineText(
                GetGameGuideText(),
                0.56f,
                Color(0.07f, 0.07f, 0.07f, 1.0f),
                0.0f,
                0.44f,
                1.40f);
        }
        else if (currentView == GuideView::ABILITIES) {
            DrawAbilityList();
            DrawAbilityDetail();
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
        else if (currentView == GuideView::ABILITIES) {
            if (AEInputCheckTriggered(AEVK_W) || AEInputCheckTriggered(AEVK_UP)) {
                ScrollAbilityList(-1);
            }
            if (AEInputCheckTriggered(AEVK_S) || AEInputCheckTriggered(AEVK_DOWN)) {
                ScrollAbilityList(+1);
            }

            s32 wheelDelta = 0;
            AEInputMouseWheelDelta(&wheelDelta);
            if (wheelDelta > 0) {
                ScrollAbilityList(-1);
            }
            else if (wheelDelta < 0) {
                ScrollAbilityList(+1);
            }

            UpdateHoveredRow();
            if (AEInputCheckTriggered(AEVK_LBUTTON)) {
                HandleAbilityRowClick();
            }
        }

        Draw();

        backButton.Update();

        if (currentView == GuideView::MENU) {
            gameGuideButton.Update();
            abilitiesGuideButton.Update();
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
        currentView = GuideView::ABILITIES;
        abilityScrollIndex = 0;
        ClampAbilityScrollIndex();
        hoveredDisplayRowIndex = -1;

        if (selectedEntryIndex < 0 && !abilityEntries.empty()) {
            selectedEntryIndex = 0;
        }
    }
}