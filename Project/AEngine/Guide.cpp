#include "Guide.hpp"

#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "CameraManager.hpp"
#include "UIManager.hpp"
#include "AudioManager.hpp"
#include "TextObject.hpp"
#include "Color.hpp"
#include "Utils.hpp"
#include "json.hpp"

#include <cmath>

namespace Guide {

    enum class GuidePage {
        GAME,
        ABILITIES
    };

    static GuidePage currentPage = GuidePage::GAME;

    static Button backButton;
    static Button gameGuideButton;
    static Button abilitiesGuideButton;

    static TextObject titleText;
    static TextObject hintText;
    static TextObject bodyLeftText;
    static TextObject bodyRightText;

    static AEGfxTexture* bgTexture = nullptr;
    static AEGfxTexture* buttonTexture = nullptr;

    static std::string abilitiesLeftColumn;
    static std::string abilitiesRightColumn;

    static std::string FormatNumber(double value) {
        if (std::fabs(value - std::round(value)) < 0.001) {
            return std::to_string(static_cast<int>(std::round(value)));
        }

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << value;
        return oss.str();
    }

    static std::string BuildAbilityLine(const nlohmann::json& ability) {
        const std::string name = ability.value("Name", "Unknown");
        const int type = ability.value("Type", 0);

        switch (type) {
        case 1: { // stats
            if (!ability.contains("Base_Stats")) {
                return name + " [STAT]";
            }

            const auto& stats = ability["Base_Stats"];
            const double hp = stats.value("HP", 0.0);
            const double atk = stats.value("ATK", 0.0);
            const double def = stats.value("DEF", 0.0);

            return name + " [STAT]  HP +" + FormatNumber(hp)
                + "  ATK +" + FormatNumber(atk)
                + "  DEF +" + FormatNumber(def);
        }

        case 2: { // offensive / projectile
            if (!ability.contains("Projectile_Stats")) {
                return name + " [OFF]";
            }

            const auto& proj = ability["Projectile_Stats"];
            const double dmg = proj.value("Base_Damage", 0.0);
            const double rad = proj.value("Radius", 0.0);
            const bool unlimited = proj.value("unlimited_charges", false);
            const int charges = proj.value("Charges", 0);

            return name + " [OFF]  DMG " + FormatNumber(dmg)
                + "  RAD " + FormatNumber(rad)
                + "  CH " + std::string(unlimited ? "INF" : std::to_string(charges));
        }

        case 3: { // defensive
            if (!ability.contains("Defensive_Stats")) {
                return name + " [DEF]";
            }

            const auto& defStats = ability["Defensive_Stats"];
            const double shield = defStats.value("ShieldHP", 0.0);

            return name + " [DEF]  Shield " + FormatNumber(shield);
        }

        default:
            return name + " [UNKNOWN]";
        }
    }

    static void BuildAbilitiesColumns() {
        abilitiesLeftColumn.clear();
        abilitiesRightColumn.clear();

        std::ifstream file("Assets/Abilities/Abilities.json");
        if (!file.is_open()) {
            abilitiesLeftColumn =
                "Could not open Assets/Abilities/Abilities.json\n"
                "Check that the file exists and the working directory is correct.";
            return;
        }

        nlohmann::json data;
        file >> data;

        std::vector<std::string> lines;
        lines.reserve(data.size());

        for (const auto& ability : data) {
            lines.push_back(BuildAbilityLine(ability));
        }

        if (lines.empty()) {
            abilitiesLeftColumn = "No abilities found in Abilities.json.";
            return;
        }

        const size_t splitIndex = (lines.size() + 1) / 2;

        for (size_t i = 0; i < lines.size(); ++i) {
            if (i < splitIndex) {
                abilitiesLeftColumn += lines[i] + "\n";
            }
            else {
                abilitiesRightColumn += lines[i] + "\n";
            }
        }
    }

    static std::string GetGameGuideText() {
        return
            "OBJECTIVE\n"
            "Destroy the enemy cannon before your own side gets destroyed.\n\n"

            "HOW TO PLAY\n"
            "1. Aim the cannon with the mouse.\n"
            "2. Hold Left Mouse Button to charge power.\n"
            "3. Release Left Mouse Button to fire.\n"
            "4. Wait until the projectile fully finishes moving.\n"
            "5. The turn then switches to the other side.\n"
            "6. When ability cards appear, click one to select it.\n\n"

            "ABILITY TYPES\n"
            "[OFF]  Offensive projectile abilities\n"
            "[STAT] Character stat boosts\n"
            "[DEF]  Defensive / utility abilities\n\n"

            "TIPS\n"
            "- Use terrain as cover instead of firing blindly.\n"
            "- Large-radius attacks are best against clustered blocks.\n"
            "- Protect the cannon support so your firing position stays safe.\n"
            "- Think about where the enemy will shoot after your turn ends.\n\n"

            "QUICK CONTROLS\n"
            "Mouse      Aim cannon\n"
            "Hold LMB   Charge shot\n"
            "Release    Fire shot\n"
            "ESC        Pause / Back\n"
            "I          Quick instructions popup\n"
            "TAB        Debug toggle";
    }

    void Load() {
        bgTexture = AssetsManager::GetTexture("Assets/game_bg.png");
        buttonTexture = AssetsManager::GetTexture("Assets/buttonTexture.png");

        BuildAbilitiesColumns();
    }

    void Initialize() {
        currentPage = GuidePage::GAME;

        CameraManager::camera.SetPosition({ 0.0f, 0.0f });
        CameraManager::camera.SetState(CameraManager::CameraController::STATIC);

        backButton = Button(
            TextObject{ "< BACK" },
            Utils::AnchorMode::CENTER,
            { -680.0f, -320.0f },
            { 220.0f, 70.0f },
            Color(0.8f, 0.2f, 0.2f, 1.0f),
            buttonTexture,
            OnBackClicked
        );

        gameGuideButton = Button(
            TextObject{ "GAME GUIDE" },
            Utils::AnchorMode::CENTER,
            { -280.0f, 300.0f },
            { 260.0f, 70.0f },
            Color(0.2f, 0.6f, 0.9f, 1.0f),
            buttonTexture,
            OnGameGuideClicked
        );

        abilitiesGuideButton = Button(
            TextObject{ "ABILITIES" },
            Utils::AnchorMode::CENTER,
            { 40.0f, 300.0f },
            { 260.0f, 70.0f },
            Color(0.55f, 0.4f, 0.9f, 1.0f),
            buttonTexture,
            OnAbilitiesGuideClicked
        );

        titleText = {
            "GUIDE - GAME",
            2.2f,
            Color(0.0f, 0.0f, 0.0f, 1.0f),
            { -0.16f, 0.74f }
        };

        hintText = {
            "1 - Game Guide  |  2 - Abilities Guide  |  ESC / BACK - Return",
            0.65f,
            Color(0.0f, 0.0f, 0.0f, 0.95f),
            { -0.98f, -0.93f }
        };
    }

    void Draw() {
        if (bgTexture != nullptr) {
            GraphicsManager::DrawWorldUI(
                bgTexture,
                GraphicsManager::CreateTransformMatrix(
                    -Config::SCREEN_WIDTH / 2.0f,
                    Config::SCREEN_HEIGHT / 2.0f,
                    0,
                    Config::SCREEN_WIDTH,
                    Config::SCREEN_HEIGHT),
                Color(1.0f, 1.0f, 1.0f, 1.0f)
            );
        }
        else {
            AEGfxSetBackgroundColor(0.1f, 0.1f, 0.15f);
        }

        UIManager::Panel mainPanel(
            { 1380.0f, 700.0f },
            Color(0.96f, 0.96f, 0.96f, 0.92f),
            Utils::AnchorMode::CENTER
        );
        mainPanel.Draw();

        titleText.TextContent(currentPage == GuidePage::GAME
            ? "GUIDE - GAME"
            : "GUIDE - ABILITIES");

        titleText.Draw();
        hintText.Draw();

        if (currentPage == GuidePage::GAME) {
            bodyLeftText = {
                GetGameGuideText(),
                0.62f,
                Color(0.05f, 0.05f, 0.05f, 1.0f),
                { -0.78f, 0.46f }
            };
            bodyLeftText.Draw();
        }
        else {
            TextObject abilitiesTitleLeft{
                "PLAYER ABILITIES (from Abilities.json)",
                0.62f,
                Color(0.05f, 0.05f, 0.05f, 1.0f),
                { -0.80f, 0.49f }
            };
            abilitiesTitleLeft.Draw();

            bodyLeftText = {
                abilitiesLeftColumn,
                0.48f,
                Color(0.05f, 0.05f, 0.05f, 1.0f),
                { -0.80f, 0.42f }
            };
            bodyRightText = {
                abilitiesRightColumn,
                0.48f,
                Color(0.05f, 0.05f, 0.05f, 1.0f),
                { 0.02f, 0.42f }
            };

            bodyLeftText.Draw();
            bodyRightText.Draw();
        }
    }

    void Update() {
        if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_BACK)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnBackClicked();
            return;
        }

        if (AEInputCheckTriggered(AEVK_1)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnGameGuideClicked();
        }

        if (AEInputCheckTriggered(AEVK_2)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnAbilitiesGuideClicked();
        }

        Draw();

        backButton.Update();
        gameGuideButton.Update();
        abilitiesGuideButton.Update();
    }

    void Free() {}

    void Unload() {}

    void OnBackClicked() {
        GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
    }

    void OnGameGuideClicked() {
        currentPage = GuidePage::GAME;
    }

    void OnAbilitiesGuideClicked() {
        currentPage = GuidePage::ABILITIES;
    }
}