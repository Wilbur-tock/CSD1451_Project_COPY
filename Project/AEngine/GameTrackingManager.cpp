#include "GameTrackingManager.hpp"
#include "LevelManager.hpp"

namespace GameTrackingManager {
	GameData gameData;

	void GameData::GameStart() {
		globalData.sessionStart = AEGetTime(nullptr);

		// Load all level tracking data
		for (int i = 0; i < Config::TOTAL_LEVEL_COUNT; ++i) {
			levelData.push_back(GameSession{});
		}
	}

	void GameData::GameEnd() {
		if (!needToUpdateLog) return;

		LevelEnd(currentLevelIdTracking);

		globalData.sessionDuration = AEGetTime(nullptr) - globalData.sessionStart;

		// Write data into a log.txt
		std::string filepath = "Assets/log.txt";
		std::ofstream ofs{ filepath };
		if (!ofs.is_open()) {
			std::cerr << "Error: Could not open file at " << filepath << std::endl;
			return;
		}

		PrintHeader(ofs, "Game Data");
		PrintData(ofs, globalData);

		for (int i = 0; i < Config::TOTAL_LEVEL_COUNT; ++i) {
			PrintHeader(ofs, "Level " + std::to_string(i+1) + " Data");
			PrintData(ofs, levelData[i]);
		}
	}

	void GameData::LevelStart(int levelId) {
		levelData[levelId].sessionStart = AEGetTime(nullptr);
		currentLevelIdTracking = levelId;
	}

	void GameData::LevelEnd(int levelId) {
		levelData[levelId].sessionDuration = AEGetTime(nullptr) - globalData.sessionStart;

		int newRounds = LevelManager::CurrentLevel()->Rounds();
		levelData[levelId].rounds = newRounds;
		globalData.rounds += newRounds;
	}

	void GameData::AddShotFired(bool isPlayer) {
		++globalData.GetGameStats(isPlayer).shotsFired;
		++levelData[currentLevelIdTracking].GetGameStats(isPlayer).shotsFired;
	}

	void GameData::AddShots(bool isPlayer, bool isHit) {
		GameStats& globalStats = globalData.GetGameStats(isPlayer);
		GameStats& levelStats = levelData[currentLevelIdTracking].GetGameStats(isPlayer);

		if (isHit) {
			++globalStats.shotsHit;
			++levelStats.shotsHit;
		}
		else {
			++globalStats.shotsMiss;
			++levelStats.shotsMiss;
		}
	}

	void GameData::AddAbilityUsed(bool isPlayer, int typeId) {
		GameStats& globalStats = globalData.GetGameStats(isPlayer);
		GameStats& levelStats = levelData[currentLevelIdTracking].GetGameStats(isPlayer);

		++globalStats.totalAbilityUsed;
		++levelStats.totalAbilityUsed;

		switch (typeId) {
		case 0:
			++globalStats.statsAbilityUsed;
			++levelStats.statsAbilityUsed;
			break;
		case 1:
			++globalStats.offensiveAbilityUsed;
			++levelStats.offensiveAbilityUsed;
			break;
		case 2:
			++globalStats.defensiveAbilityUsed;
			++levelStats.defensiveAbilityUsed;
			break;
		}
	}

	void GameData::PrintHeader(std::ofstream& ofs, std::string title) {
		size_t stringSize = 75 - title.size();

		std::string border(stringSize/2, '=');
		ofs << border << title << border;

		if (stringSize % 2 != 0) ofs << '=';
	}

	void GameData::PrintData(std::ofstream& ofs, GameSession gameSession) {
		int mins = static_cast<int>(gameSession.sessionDuration) / 60;
		int secs = static_cast<int>(gameSession.sessionDuration) % 60;

		// Print session data
		ofs << std::left << std::setw(width) << "\nGame Duration: " << mins << " mins " << secs << " secs" 
			<< std::left << std::setw(width) << "\nTotal Rounds Played: " << gameSession.rounds
			<< std::endl;

		PrintHeader(ofs, "Player Data");
		PrintGameStats(ofs, gameSession.GetGameStats(true));
		PrintHeader(ofs, "Enemy Data");
		PrintGameStats(ofs, gameSession.GetGameStats(false));
	}

	void GameData::PrintGameStats(std::ofstream& ofs, GameStats stats) {
		ofs << std::left << std::setw(width) << "\nTotal Shots: " << stats.shotsFired
			<< std::left << std::setw(width) << "\nHits: " << stats.shotsHit
			<< std::left << std::setw(width) << "\nMiss: " << stats.shotsMiss <<'\n'
			<< std::left << std::setw(width) << "\nTotal Ability Used: " << stats.totalAbilityUsed
			<< std::left << std::setw(width) << "\nStats Ability Used: " << stats.statsAbilityUsed
			<< std::left << std::setw(width) << "\nOffensive Ability Used: " << stats.offensiveAbilityUsed
			<< std::left << std::setw(width) << "\nDefensive Ability Used: " << stats.defensiveAbilityUsed
			<< '\n' << std::endl;
	}
}