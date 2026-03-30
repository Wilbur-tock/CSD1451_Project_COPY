/*
Authors: Nixon
Description: Manage and draw the leve game over results UI

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "ResultsUI.hpp"

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

	static void OnResultRestart() {
		GameStateManager::next = GameStateManager::GameState::RESTART;
	}

	static void OnResultNextLevel() {
		Level* currentLevel = LevelManager::CurrentLevel();
		if (!currentLevel) return;

		const int nextLevelIndex = currentLevel->Id() + 1;

		if (nextLevelIndex < 0 || nextLevelIndex >= Config::TOTAL_LEVEL_COUNT) return; // level index out of range
		if (!LevelManager::IsLevelUnlocked(nextLevelIndex)) return;

		// proceed to next level
		int levelId = LevelManager::CurrentLevel()->Id() + 1;
		GameStateManager::GameState nextGameState = static_cast<GameStateManager::GameState>(static_cast<int>(GameStateManager::GameState::LEVEL_0) + levelId);

		// if finished final level back to main menu (addtional checks not really needed)
		if (levelId >= Config::TOTAL_LEVEL_COUNT) {
			nextGameState = GameStateManager::GameState::MAIN_MENU;
		}

		GameStateManager::next = nextGameState;
	}

	static void OnResultMainMenu() {
		GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
	}

	static int GetAccuracyPercent(GameTrackingManager::CombatStatsSnapshot const& stats) {
		if (stats.shotsFired == 0) return 0;

		float ratio = static_cast<float>(stats.shotsHit) / static_cast<float>(stats.shotsFired);
		return static_cast<int>(ratio * 100.0f + 0.5f);
	}

	static std::vector<std::string> BuildResultSummaryLines() {
		std::vector<std::string> lines;

		Level* currentLevel = LevelManager::CurrentLevel();
		if (!currentLevel) {
			lines.push_back("No level data available.");
			return lines;
		}

		GameTrackingManager::CombatStatsSnapshot const playerStats =
			GameTrackingManager::gameData.GetCurrentLevelStats(true);
		GameTrackingManager::CombatStatsSnapshot const enemyStats =
			GameTrackingManager::gameData.GetCurrentLevelStats(false);

		DamageLog const& playerDamage = currentLevel->GetDamageLog(true);
		DamageLog const& enemyDamage = currentLevel->GetDamageLog(false);

		lines.push_back("Rounds Played: " + std::to_string(currentLevel->Rounds()));
		lines.push_back("");
		lines.push_back("PLAYER vs ENEMY");
		lines.push_back("Shots Fired   - You: " + std::to_string(playerStats.shotsFired) + " | Enemy: " + std::to_string(enemyStats.shotsFired));
		lines.push_back("Hits          - You: " + std::to_string(playerStats.shotsHit) + " | Enemy: " + std::to_string(enemyStats.shotsHit));
		lines.push_back("Misses        - You: " + std::to_string(playerStats.shotsMiss) + " | Enemy: " + std::to_string(enemyStats.shotsMiss));
		lines.push_back("Accuracy      - You: " + std::to_string(GetAccuracyPercent(playerStats)) + "% | Enemy: " + std::to_string(GetAccuracyPercent(enemyStats)) + "%");
		lines.push_back("Abilities Used- You: " + std::to_string(playerStats.totalAbilityUsed) + " | Enemy: " + std::to_string(enemyStats.totalAbilityUsed));
		lines.push_back("Tile Damage   - You: " + std::to_string(playerDamage.totalTileDamage) + " | Enemy: " + std::to_string(enemyDamage.totalTileDamage));
		lines.push_back("Char Damage   - You: " + std::to_string(playerDamage.totalCharacterDamage) + " | Enemy: " + std::to_string(enemyDamage.totalCharacterDamage));

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

	void Intialize() {
		// Initialize ResultsUI
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
			OnResultRestart
		);

		resultNextLevelButton = Button(
			TextObject{ "NEXT LEVEL" },
			Utils::AnchorMode::CENTER,
			{ nextX, buttonY },
			{ btnW, btnH },
			Color(0.2f, 0.75f, 0.3f, 1.0f),
			btnTex,
			nullptr,
			OnResultNextLevel
		);

		resultMainMenuButton = Button(
			TextObject{ "MAIN MENU" },
			Utils::AnchorMode::CENTER,
			{ menuX, buttonY },
			{ btnW, btnH },
			Color(0.8f, 0.2f, 0.2f, 1.0f),
			btnTex,
			nullptr,
			OnResultMainMenu
		);

		// Dark overlay
		resultOverlay = UIManager::Panel{
			{ (float)Config::SCREEN_WIDTH, (float)Config::SCREEN_HEIGHT },
			Color(0.0f, 0.0f, 0.0f, 0.9f),
			Utils::AnchorMode::TOP_LEFT
		};

		// Slightly wider panel now because of 3 buttons
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

	void Update(LevelStatus status) {
		const bool isShowingResults =
			(status == LevelStatus::PLAYER_WON || status == LevelStatus::ENEMY_WON);

		Utils::isResultScreenOn = isShowingResults;

		if (!isShowingResults) {
			return;
		}

		Draw(status);

		// Debug Commands for results UI
		if (AEInputCheckTriggered(AEVK_N)) {
			AudioManager::PlaySFX(AudioManager::SFX::UIButton);
			OnResultNextLevel();
		}

		if (AEInputCheckTriggered(AEVK_R)) {
			AudioManager::PlaySFX(AudioManager::SFX::UIButton);
			OnResultRestart();
		}
	}

	void Draw(LevelStatus status) {
		resultOverlay.Draw();

		Level* currentLevel = LevelManager::CurrentLevel();
		if (!currentLevel) {
			return;
		}

		int nextLevelId = currentLevel->Id() + 1;
		bool const showNextLevelButton = nextLevelId < Config::TOTAL_LEVEL_COUNT && status == LevelStatus::PLAYER_WON;

		Color panelColor = (status == LevelStatus::PLAYER_WON)
			? Color(0.1f, 0.3f, 0.1f, 0.9f)
			: Color(0.3f, 0.1f, 0.1f, 0.9f);

		centerPanel.SetColor(panelColor);
		centerPanel.Draw();

		// -- Draw result title text --
		std::string resultStr = (status == LevelStatus::PLAYER_WON) ? "VICTORY!" : "DEFEAT!";
		Color textColor = (status == LevelStatus::PLAYER_WON)
			? Color(0.2f, 1.0f, 0.2f, 1.0f)
			: Color(1.0f, 0.2f, 0.2f, 1.0f);

		resultTitle.TextContent(resultStr);
		resultTitle.SetColor(textColor);
		resultTitle.Draw();

		// -- Draw subtitle text --
		std::string subStr;
		if (status == LevelStatus::PLAYER_WON) {
			subStr = showNextLevelButton ? "Level cleared! Press NEXT LEVEL to continue.  [N]" : "You cleared the final level!";
		}
		else {
			subStr = "All your units have fallen!";
		}

		subtitle.TextContent(subStr);
		subtitle.Draw();

		DrawResultSummary();

		// -- Draw buttons --
		resultRestartButton.Update();

		if (showNextLevelButton) {
			resultNextLevelButton.Update();
		}

		resultMainMenuButton.Update();
	}
}