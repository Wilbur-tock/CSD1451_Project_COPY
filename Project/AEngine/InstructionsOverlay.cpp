#include "InstructionsOverlay.hpp"
#include "UIManager.hpp"
#include "TextObject.hpp"
#include "Utils.hpp"
#include "Color.hpp"
#include "GameStateManager.hpp"

namespace UIInstructionsOverlay {

    static TextObject sInstructionText;

    static std::string GetStateName(GameStateManager::GameState s) {
        using GameStateManager::GameState;
        switch (s) {
        case GameState::INITIALIZE: return "INITIALIZE";
        case GameState::MAIN_MENU:  return "MAIN MENU";
        case GameState::MAP_EDITOR: return "MAP EDITOR";
        case GameState::LEVEL_1:    return "LEVEL 1";
        case GameState::LEVEL_2:    return "LEVEL 2";
        case GameState::CREDITS:    return "CREDITS";
        case GameState::RESTART:    return "RESTART";
        case GameState::EXIT:       return "EXIT";
        case GameState::QUIT:       return "QUIT";
        }
        return "GAME";
    }

    void Draw(GameStateManager::GameState current) {
        UIManager::Panel dim{
            {static_cast<float>(Config::SCREEN_WIDTH), static_cast<float>(Config::SCREEN_HEIGHT)},
            Color(0.0f, 0.0f, 0.0f, 0.55f),
            Utils::AnchorMode::CENTER
        };
        dim.Draw();

        UIManager::Panel panel{
            {1120.0f, 730.0f},
            Color(0.12f, 0.12f, 0.16f, 0.95f),
            Utils::AnchorMode::CENTER
        };
        panel.Draw();

        std::string msg =
            "INSTRUCTIONS  (Press [I] to close)\n"
            "---------------------------------\n"
            "Current: " + GetStateName(current) + "\n\n"
            "WHILE THIS POPUP IS OPEN\n"
            "[I] Close this popup\n"
            "[ESC] Quit Game\n\n"
            "WHEN THIS POPUP IS CLOSED\n"
            "[P] Toggle Debugger\n"
            "[M] Main Menu\n"
            "[ESC] Pause Menu\n\n";                                        // <-- CHANGED

        using GameStateManager::GameState;
        if (current == GameState::LEVEL_1 || current == GameState::LEVEL_2) {
            msg +=
                "LEVEL\n"
                "Mouse: Aim cannon\n"
                "Hold [LMB]: Charge shot\n"
                "Release [LMB]: Fire\n"
                "Ability selection: Click a card when the selector appears\n\n"
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
                "[CTRL] + [1..5]: Load level file\n\n"
                "NOTE: Editor input is blocked while this popup is open.\n";
        }
        else if (current == GameState::MAIN_MENU) {
            msg +=
                "MAIN MENU\n"
                "[1] Play\n"
                "[2] Map Editor\n"
                "[3] Credits\n"
                "[Q] Quit\n";
        }
        else if (current == GameState::CREDITS) {
            msg +=
                "CREDITS\n"
                "Close this popup, then press [M] to return to Main Menu.\n";
        }

        float x = panel.Pos().x + 40.0f;
        float y = panel.Pos().y - 40.0f;
        Utils::NormalizeCoordinates(x, y);

        sInstructionText.textContent = msg;
        sInstructionText.textScale = 0.65f;
        sInstructionText.color = { 1.0f, 1.0f, 1.0f, 1.0f };
        sInstructionText.pos = { x, y };
        sInstructionText.Draw();
    }
}