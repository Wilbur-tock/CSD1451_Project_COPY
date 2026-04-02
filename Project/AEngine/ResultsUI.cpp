/*
Authors: Nixon
Description: Manage and draw the level game over results UI

All content (C) 2026 DigiPen Institute of Technology Singapore. All Rights Reserved.
*/

#include "ResultsUI.hpp"
#include "ConfirmDialog.hpp"

#include "UIManager.hpp"
#include "AssetsManager.hpp"
#include "AudioManager.hpp"
#include "GameStateManager.hpp"
#include "GameTrackingManager.hpp"

#include <sstream>
#include <iomanip>
#include <vector>

namespace ResultsUI {
    // Results UI objects
    UIManager::Panel resultOverlay, centerPanel;
    Button resultRestartButton, resultNextLevelButton, resultMainMenuButton;
    TextObject resultTitle, subtitle;

    // -----------------------------------------------------------------------
    // Internal callbacks
    //
    // OnResultRestart and OnResultMainMenu are destructive -- they discard the
    // current match. Both now route through ConfirmDialog instead of acting
    // immediately. OnResultNextLevel is not destructive (the match is already
    // over) so it fires directly without a dialog.
    // -----------------------------------------------------------------------

    static void OnResultRestart() {
        // Restarting re-plays the same level, discarding the current result.
        ConfirmDialog::Open(
            "The current match result will be discarded.",
            "Are you sure you want to restart?",
            []() {
                GameStateManager::next = GameStateManager::GameState::RESTART;
            }
        );
    }

    static void OnResultNextLevel() {
        // Not destructive: the level is already finished.
        Level* currentLevel = LevelManager::CurrentLevel();
        if (!currentLevel) return;

        const int nextLevelIndex = currentLevel->Id() + 1;
        if (nextLevelIndex < 0 || nextLevelIndex >= Config::TOTAL_LEVEL_COUNT) return;
        if (!LevelManager::IsLevelUnlocked(nextLevelIndex)) return;

        int levelId = LevelManager::CurrentLevel()->Id() + 1;
        GameStateManager::GameState nextGameState = static_cast<GameStateManager::GameState>(
            static_cast<int>(GameStateManager::GameState::LEVEL_0) + levelId
            );

        if (levelId >= Config::TOTAL_LEVEL_COUNT) {
            nextGameState = GameStateManager::GameState::MAIN_MENU;
        }

        GameStateManager::next = nextGameState;
    }

    static void OnResultMainMenu() {
        // Going back to the main menu from the results screen discards the result.
        ConfirmDialog::Open(
            "The current match result will be discarded.",
            "Are you sure you want to return to the main menu?",
            []() {
                GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
            }
        );
    }

    // -----------------------------------------------------------------------
    // Stat summary helpers (unchanged)
    // -----------------------------------------------------------------------

    static std::vector<std::string> BuildResultSummaryLines() {
        std::vector<std::string> lines;

        Level* currentLevel = LevelManager::CurrentLevel();
        if (!currentLevel) {
            lines.push_back("No level data available.");
            return lines;
        }

        GameTrackingManager::GameStats const playerStats = GameTrackingManager::gameData.GetCurrentLevelStats(true);
        GameTrackingManager::GameStats const enemyStats = GameTrackingManager::gameData.GetCurrentLevelStats(false);

        lines.push_back("Rounds Played: " + std::to_string(currentLevel->Rounds()));
        lines.push_back("");
        lines.push_back("PLAYER vs ENEMY");
        lines.push_back("Shots Fired   - You: " + std::to_string(playerStats.shotsFired) + " | Enemy: " + std::to_string(enemyStats.shotsFired));
        lines.push_back("Hits          - You: " + std::to_string(playerStats.shotsHit) + " | Enemy: " + std::to_string(enemyStats.shotsHit));
        lines.push_back("Misses        - You: " + std::to_string(playerStats.shotsMiss) + " | Enemy: " + std::to_string(enemyStats.shotsMiss));
        lines.push_back("Accuracy      - You: " + Utils::FormatFloat(playerStats.shotsAccuracy, 2) + "% | Enemy: " + Utils::FormatFloat(enemyStats.shotsAccuracy, 2) + "%");
        lines.push_back("Abilities Used- You: " + std::to_string(playerStats.totalAbilityUsed) + " | Enemy: " + std::to_string(enemyStats.totalAbilityUsed));
        lines.push_back("Tile Damage   - You: " + std::to_string(playerStats.totalDamage) + " | Enemy: " + std::to_string(enemyStats.totalDamage));
        lines.push_back("Char Damage   - You: " + std::to_string(playerStats.characterDamage) + " | Enemy: " + std::to_string(playerStats.characterDamage));

        return lines;
    }

    static void DrawResultSummary() {
        std::vector<std::string> lines = BuildResultSummaryLines();

        float startX = -0.16f;
        float startY = 0.15f;
        float lineStep = 0.042f;

        for (size_t i = 0; i < lines.size(); ++i) {
            AEGfxPrint(
                AssetsManager::textFont,
                lines[i].c_str(),
                startX,
                startY - static_cast<float>(i) * lineStep,
                0.65f, 0.75f,
                1.0f, 1.0f, 1.0f
            );
        }
    }

    // -----------------------------------------------------------------------
    // Intialize (matches original spelling to avoid breaking call sites)
    // -----------------------------------------------------------------------

    void Intialize() {
        AEGfxTexture* btnTex = AssetsManager::GetTexture("Assets/buttonTexture.png");
        if (!btnTex) btnTex = AssetsManager::squareTexture;

        float btnW = 220.0f;
        float btnH = 70.0f;
        float gap = 25.0f;

        // 3-button layout: [RESTART] [NEXT LEVEL] [MAIN MENU]
        float totalW = btnW * 3.0f + gap * 2.0f;
        float startX = -totalW * 0.5f;
        float buttonY = -125.0f;

        float restartX = startX;
        float nextX = restartX + btnW + gap;
        float menuX = nextX + btnW + gap;

        resultRestartButton = Button(
            TextObject{ "RESTART" },
            Utils::AnchorMode::CENTER,
            { restartX, buttonY },
            { btnW, btnH },
            Color(0.2f, 0.6f, 0.9f, 1.0f),
            btnTex,
            nullptr,
            OnResultRestart // now routes through ConfirmDialog
        );

        resultNextLevelButton = Button(
            TextObject{ "NEXT LEVEL" },
            Utils::AnchorMode::CENTER,
            { nextX, buttonY },
            { btnW, btnH },
            Color(0.2f, 0.75f, 0.3f, 1.0f),
            btnTex,
            nullptr,
            OnResultNextLevel // not destructive -- no confirmation
        );

        resultMainMenuButton = Button(
            TextObject{ "MAIN MENU" },
            Utils::AnchorMode::CENTER,
            { menuX, buttonY },
            { btnW, btnH },
            Color(0.8f, 0.2f, 0.2f, 1.0f),
            btnTex,
            nullptr,
            OnResultMainMenu // now routes through ConfirmDialog
        );

        // Dark overlay
        resultOverlay = UIManager::Panel{
            { (float)Config::SCREEN_WIDTH, (float)Config::SCREEN_HEIGHT },
            Color(0.0f, 0.0f, 0.0f, 0.9f),
            Utils::AnchorMode::TOP_LEFT
        };

        // Slightly wider panel to fit 3 buttons
        centerPanel = UIManager::Panel{
            { 920.0f, 430.0f },
            Color{ 1,1,1 },
            Utils::AnchorMode::CENTER
        };

        resultTitle = TextObject{
            "", 2.0f, Color{ 1,1,1 }, { {0,0}, {0,0} }, Utils::AnchorMode::CENTER
        };
        resultTitle.SetPosition(centerPanel.GetTransform());
        resultTitle.SetOffsetPosition(0, 140.0f);

        subtitle = TextObject{
            "", 0.7f, Color{ 1,1,1, 0.9f }, { {0,0}, {0,0} }, Utils::AnchorMode::CENTER
        };
        subtitle.SetPosition(centerPanel.GetTransform());
        subtitle.SetOffsetPosition(0, 95.0f);
    }

    // -----------------------------------------------------------------------
    // Update
    // -----------------------------------------------------------------------

    void Update(LevelStatus status) {
        const bool isShowingResults =
            (status == LevelStatus::PLAYER_WON || status == LevelStatus::ENEMY_WON);

        Utils::isResultScreenOn = isShowingResults;

        if (!isShowingResults) return;

        Draw(status);

        // -------------------------------------------------------------------
        // Confirm dialog intercept
        //
        // If OnResultRestart or OnResultMainMenu opened the dialog, update it
        // and return early so the result-screen buttons do not receive clicks
        // through the dialog overlay.
        // -------------------------------------------------------------------
        if (ConfirmDialog::IsOpen()) {
            ConfirmDialog::Update();
            return;
        }

        // Debug shortcuts (only active when dialog is not open)
        if (AEInputCheckTriggered(AEVK_N)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnResultNextLevel();
        }
        if (AEInputCheckTriggered(AEVK_R)) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            OnResultRestart(); // will open the confirm dialog next frame
        }
    }

    // -----------------------------------------------------------------------
    // Draw
    // -----------------------------------------------------------------------

    void Draw(LevelStatus status) {
        resultOverlay.Draw();

        Level* currentLevel = LevelManager::CurrentLevel();
        if (!currentLevel) return;

        int  nextLevelId = currentLevel->Id() + 1;
        bool showNextLevelButton = nextLevelId < Config::TOTAL_LEVEL_COUNT
            && status == LevelStatus::PLAYER_WON;

        Color panelColor = (status == LevelStatus::PLAYER_WON)
            ? Color(0.1f, 0.3f, 0.1f, 0.9f)
            : Color(0.3f, 0.1f, 0.1f, 0.9f);

        centerPanel.SetColor(panelColor);
        centerPanel.Draw();

        // Result title
        std::string resultStr = (status == LevelStatus::PLAYER_WON) ? "VICTORY!" : "DEFEAT!";
        Color textColor = (status == LevelStatus::PLAYER_WON)
            ? Color(0.2f, 1.0f, 0.2f, 1.0f)
            : Color(1.0f, 0.2f, 0.2f, 1.0f);

        resultTitle.TextContent(resultStr);
        resultTitle.SetColor(textColor);
        resultTitle.Draw();

        // Subtitle
        std::string subStr;
        if (status == LevelStatus::PLAYER_WON) {
            subStr = showNextLevelButton
                ? "Level cleared! Press NEXT LEVEL to continue.  [N]"
                : "You cleared the final level!";
        }
        else {
            subStr = "All your units have fallen!";
        }

        subtitle.TextContent(subStr);
        subtitle.Draw();

        DrawResultSummary();

        // Buttons
        resultRestartButton.Update();
        if (showNextLevelButton) {
            resultNextLevelButton.Update();
        }
        resultMainMenuButton.Update();
    }

} // namespace ResultsUI