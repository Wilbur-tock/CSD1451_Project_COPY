#include "ResultsUI.hpp"

#include "UIManager.hpp"
#include "AssetsManager.hpp"
#include "AudioManager.hpp"
#include "GameStateManager.hpp"

namespace ResultsUI {
	// Results UI objects
	bool resultScreenInitialized = false;
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
		GameStateManager::GameState nextGameState = static_cast<GameStateManager::GameState>(static_cast<int>(GameStateManager::GameState::LEVEL_1) + levelId);

		// if finished final level back to main menu (addtional checks not really needed)
		if (levelId >= Config::TOTAL_LEVEL_COUNT) {
			nextGameState = GameStateManager::GameState::MAIN_MENU;
		}

		GameStateManager::next = nextGameState;
	}

	static void OnResultMainMenu() {
		GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
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
		float buttonY = -35.0f;

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
			{ 820.0f, 320.0f },
			Color{ 1,1,1 },
			Utils::AnchorMode::CENTER
		};

		resultTitle = TextObject{
			"", 2.0f, Color{ 1,1,1 }, { {0,0}, {0,0} }, Utils::AnchorMode::CENTER
		};
		resultTitle.SetPosition(centerPanel.GetTransform());
		resultTitle.SetOffsetPosition(0, 70.0f);

		subtitle = TextObject{
			"", 0.7f, Color{ 1,1,1, 0.9f }, { {0,0}, {0,0} }, Utils::AnchorMode::CENTER
		};
		subtitle.SetPosition(centerPanel.GetTransform());
		subtitle.SetOffsetPosition(0, 10.0f);
	}

	void Update(LevelStatus status) {
		if (status == LevelStatus::PLAYER_WON || status == LevelStatus::ENEMY_WON) {
			Utils::isResultScreenOn = true;
			Draw(status);
		}
		else {
			Utils::isResultScreenOn = false;
		}

		// Debug Commands for results UI
		if (AEInputCheckTriggered(AEVK_N)) {
			AudioManager::PlaySFX(AudioManager::SFX::UIButton);
			OnResultNextLevel();
		}
	}

	void Draw(LevelStatus status) {
		resultOverlay.Draw();

		int nextLevelId = LevelManager::CurrentLevel()->Id() + 1;
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
			subStr = "All your units have fallen!  [R]";
		}

		subtitle.TextContent(subStr);
		subtitle.Draw();

		// -- Draw buttons --
		resultRestartButton.Update();

		if (showNextLevelButton) {
			resultNextLevelButton.Update();
		}

		resultMainMenuButton.Update();
	}
}