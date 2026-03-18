#include "InstructionsOverlay.hpp"
#include "UIManager.hpp"
#include "Utils.hpp"
#include "Color.hpp"
#include "GameStateManager.hpp"
#include "LevelManager.hpp"

namespace UIInstructionsOverlay {

    static std::string GetStateName(GameStateManager::GameState s) {
        using GameStateManager::GameState;
        switch (s) {
        case GameState::INITIALIZE: return "INITIALIZE";
        case GameState::MAIN_MENU:  return "MAIN MENU";
        case GameState::LEVEL_SELECT: return "LEVEL SELECT";
        case GameState::MAP_EDITOR: return "MAP EDITOR";
        case GameState::LEVEL:
            if (LevelManager::CurrentLevel()) {
                return LevelManager::CurrentLevel()->Name();
            }
            return "LEVEL";
        case GameState::CREDITS:    return "CREDITS";
        case GameState::SETTINGS:   return "SETTINGS";
        case GameState::GUIDE:      return "GUIDE";
        case GameState::RESTART:    return "RESTART";
        case GameState::EXIT:       return "EXIT";
        case GameState::QUIT:       return "QUIT";
        }
        return "GAME";
    }

    UIManager::Panel dimPanel, mainPanel;
    TextObject instructionText;

    void Initialize() {
        dimPanel = UIManager::Panel{
            {static_cast<float>(Config::SCREEN_WIDTH), static_cast<float>(Config::SCREEN_HEIGHT)},
            Color(0.0f, 0.0f, 0.0f, 0.55f),
            Utils::AnchorMode::CENTER
        };

        mainPanel = UIManager::Panel{
            {1120.0f, 730.0f},
            Color(0.12f, 0.12f, 0.16f, 0.95f),
            Utils::AnchorMode::CENTER
        };

        float x = mainPanel.Pos().x + 40.0f;
        float y = mainPanel.Pos().y - 40.0f;
        Utils::NormalizeCoordinates(x, y);

        instructionText = TextObject{ "", 0.65f, Color{1,1,1}, {x,y} };
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

        if (current == GameState::LEVEL) {
            msg +=
                "LEVEL\n"
                "Mouse: Aim cannon\n"
                "Hold [LMB]: Charge shot\n"
                "Release [LMB]: Fire\n"
                "Ability selection: Click a card when the selector appears\n"
                "[ESC]: Pause Menu\n\n"
                "NOTE: Gameplay is paused while this popup is open.\n";
        }
        else if (current == GameState::MAP_EDITOR) {
            msg +=
                "MAP EDITOR\n"
                "[LMB]: Place selected tile\n"
                "[RMB]: Erase tile\n"
                "[CTRL] + Click: Fill ROW\n"
                "[SHIFT] + Click: Fill COLUMN\n"
                "[Q] / [E]: Change selected tile\n"
                "[T]: Toggle tile list\n"
                "[CTRL] + [S]: Save current map\n"
                "[R]: Reset map\n"
                "[CTRL] + [1..5]: Load level file\n"
                "[ESC]: Pause Menu\n\n"
                "NOTE: Editor input is blocked while this popup is open.\n";
        }
        else if (current == GameState::MAIN_MENU) {
            msg +=
                "MAIN MENU\n"
                "[1] Play\n"
                "[2] Map Editor\n"
                "[3] Settings\n"
                "[4] Guide\n"
                "[5] Credits\n"
                "[Q] Quit\n";
        }
        else if (current == GameState::LEVEL_SELECT) {
            msg += "LEVEL SELECT\n";

            int levelCount = LevelManager::LevelCount();
            int quickSelectCount = (levelCount < 9) ? levelCount : 9;

            for (int i = 0; i < quickSelectCount; ++i) {
                msg += "[" + std::to_string(i + 1) + "] Start " + LevelManager::GetLevelName(i) + "\n";
            }

            if (levelCount > 9) {
                msg += "Mouse: Start any level button\n";
            }

            if (levelCount <= 0) {
                msg += "No playable levels found.\n";
            }

            msg += "[ESC] / [BACK]: Return to Main Menu\n";
        }
        else if (current == GameState::GUIDE) {
            msg +=
                "GUIDE\n"
                "[1] Game Guide page\n"
                "[2] Abilities Guide page\n"
                "[ESC] / [BACK]: Return to Main Menu\n";
        }
        else if (current == GameState::CREDITS) {
            msg +=
                "CREDITS\n"
                "[SPACE]: Speed up scroll\n"
                "[ESC] / [BACK]: Return to Main Menu\n";
        }
        else if (current == GameState::SETTINGS) {
            msg +=
                "SETTINGS\n"
                "Use the sliders or +/- buttons to adjust Master, BGM, and SFX volume.\n"
                "[ESC] / [BACK]: Return\n";
        }

        instructionText.TextContent(msg);
        instructionText.Draw();
    }
}