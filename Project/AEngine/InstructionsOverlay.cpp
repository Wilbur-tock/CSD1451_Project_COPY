#include "InstructionsOverlay.hpp"
#include "UIManager.hpp"
#include "Utils.hpp"
#include "Color.hpp"
#include "GameStateManager.hpp"
#include "Config.hpp"
#include "LevelManager.hpp"

namespace UIInstructionsOverlay {

    namespace {
        bool IsGameplayLevel(GameStateManager::GameState s) {
            using GameStateManager::GameState;
            switch (s) {
            case GameState::LEVEL_1:
            case GameState::LEVEL_2:
            case GameState::LEVEL_3:
                return true;
            default:
                return false;
            }
        }

        std::string GetStateName(GameStateManager::GameState s) {
            using GameStateManager::GameState;
            switch (s) {
            case GameState::INITIALIZE:   return "INITIALIZE";
            case GameState::MAIN_MENU:    return "MAIN MENU";
            case GameState::LEVEL_SELECT: return "LEVEL SELECT";
            case GameState::MAP_EDITOR:   return "MAP EDITOR";
            case GameState::LEVEL_1:      return "LEVEL 1";
            case GameState::LEVEL_2:      return "LEVEL 2";
            case GameState::LEVEL_3:      return "LEVEL 3";
            case GameState::CREDITS:      return "CREDITS";
            case GameState::SETTINGS:     return "SETTINGS";
            case GameState::GUIDE:        return "GUIDE";
            case GameState::RESTART:      return "RESTART";
            case GameState::EXIT:         return "EXIT";
            case GameState::QUIT:         return "QUIT";
            }
            return "GAME";
        }
    }

    UIManager::Panel dimPanel, mainPanel;
    TextObject instructionText;

    void Initialize() {
        dimPanel = UIManager::Panel{
            { static_cast<float>(Config::SCREEN_WIDTH), static_cast<float>(Config::SCREEN_HEIGHT) },
            Color(0.0f, 0.0f, 0.0f, 0.55f),
            Utils::AnchorMode::CENTER
        };

        mainPanel = UIManager::Panel{
            { 1120.0f, 730.0f },
            Color(0.12f, 0.12f, 0.16f, 0.95f),
            Utils::AnchorMode::CENTER
        };

        float x = mainPanel.GetTransform().pos.x + 40.0f;
        float y = mainPanel.GetTransform().pos.y - 40.0f;
        Utils::NormalizeCoordinates(x, y);

        instructionText = TextObject{ "", 0.65f, Color{1,1,1}, {x, y} };
    }

    void Draw() {
        GameStateManager::GameState current = GameStateManager::current;

        dimPanel.Draw();
        mainPanel.Draw();

        std::string msg =
            "INSTRUCTIONS  (Press [I] to close)\n"
            "---------------------------------\n"
            "Current: " + GetStateName(current) + "\n\n"
            "WHILE THIS POPUP IS OPEN\n"
            "[I] Close this popup\n\n"
            "WHEN THIS POPUP IS CLOSED\n"
            "[TAB] Toggle Debugger\n";

        using GameStateManager::GameState;

        if (IsGameplayLevel(current)) {
            msg +=
                "\nLEVEL\n"
                "Mouse: Aim cannon\n"
                "Hold [LMB]: Charge shot\n"
                "Release [LMB]: Fire\n"
                "Ability selection: Click a card when the selector appears\n"
                "[ESC]: Pause Menu\n\n"
                "NOTE: Gameplay is paused while this popup is open.\n";
        }
        else if (current == GameState::MAP_EDITOR) {
            msg +=
                "\nMAP EDITOR\n"
                "[LMB]: Place selected tile\n"
                "[RMB]: Erase tile\n"
                "[CTRL] + Click: Fill ROW\n"
                "[SHIFT] + Click: Fill COLUMN\n"
                "[Q] / [E]: Change selected tile\n"
                "[T]: Toggle tile list\n"
                "[CTRL] + [S]: Save current map\n"
                "[R]: Reset map\n"
                "[CTRL] + [1..3]: Load level file\n"
                "[ESC]: Pause Menu\n\n"
                "NOTE: Editor input is blocked while this popup is open.\n";
        }
        else if (current == GameState::MAIN_MENU) {
            msg +=
                "\nMAIN MENU\n"
                "[1] Play\n"
                "[2] Map Editor\n"
                "[3] Settings\n"
                "[4] Guide\n"
                "[5] Credits\n"
                "[Q] Quit\n";
        }
        else if (current == GameState::LEVEL_SELECT) {
            msg +=
                "\nLEVEL SELECT\n"
                "[" + std::to_string(1) + "-" + std::to_string(LevelManager::TOTAL_LEVEL_COUNT) + "] Start unlocked level\n"
                "[ESC] / [BACK]: Return to Main Menu\n";
        }
        else if (current == GameState::GUIDE) {
            msg +=
                "\nGUIDE\n"
                "[1] Open Game Guide\n"
                "[2] Open Abilities Guide\n"
                "Inside Abilities Guide:\n"
                "Click an ability row to inspect it\n"
                "[W]/[S] or [UP]/[DOWN]: Scroll ability list\n"
                "[ESC] / [BACK]: Return to Guide Menu or Main Menu\n";
        }
        else if (current == GameState::CREDITS) {
            msg +=
                "\nCREDITS\n"
                "[SPACE]: Speed up scroll\n"
                "[ESC] / [BACK]: Return to Main Menu\n";
        }
        else if (current == GameState::SETTINGS) {
            msg +=
                "\nSETTINGS\n"
                "Use the sliders or +/- buttons to adjust Master, BGM, and SFX volume.\n"
                "[ESC] / [BACK]: Return\n";
        }

        instructionText.TextContent(msg);
        instructionText.Draw();
    }
}