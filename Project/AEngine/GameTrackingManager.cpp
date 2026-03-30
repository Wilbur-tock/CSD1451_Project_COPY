/*
Authors: Dayrius
Description: Track game data throughout the whole game and save the data to a file

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "GameTrackingManager.hpp"
#include "LevelManager.hpp"

namespace GameTrackingManager {
	GameData gameData;

	void GameData::GameStart() {
		globalData.sessionStart = AEGetTime(nullptr);

		levelData.clear();
		levelData.reserve(Config::TOTAL_LEVEL_COUNT);

		for (int i = 0; i < Config::TOTAL_LEVEL_COUNT; ++i) {
			levelData.push_back(GameSession{});
		}
	}

	void GameData::GameEnd() {
		globalData.sessionDuration = AEGetTime(nullptr) - globalData.sessionStart;

		if (LevelManager::CurrentLevel()) {
			LevelEnd(currentLevelIdTracking);
		}

		if (needToUpdateLog) {
			std::string filepath = "Assets/Data/gameLog.txt";

			std::ofstream ofs(filepath, std::ios::app);
			if (ofs.is_open()) {
				PrintHeader(ofs, "All game data");
				PrintData(ofs, globalData);

				for (int i = 0; i < static_cast<int>(levelData.size()); ++i) {
					PrintHeader(ofs, "Level " + std::to_string(i + 1) + " data");
					PrintData(ofs, levelData[i]);
				}

				ofs.close();
			}
		}

		needToUpdateLog = false;
	}

	void GameData::LevelStart(int levelId) {
		if (levelId < 0 || levelId >= static_cast<int>(levelData.size())) {
			return;
		}

		levelData[levelId].sessionStart = AEGetTime(nullptr);
		currentLevelIdTracking = levelId;
	}

	void GameData::LevelEnd(int levelId) {
		if (levelId < 0 || levelId >= static_cast<int>(levelData.size())) {
			return;
		}

		levelData[levelId].sessionDuration = AEGetTime(nullptr) - levelData[levelId].sessionStart;
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

	CombatStatsSnapshot GameData::GetLevelStats(int levelId, bool isPlayer) const {
		CombatStatsSnapshot snapshot{};

		if (levelId < 0 || levelId >= static_cast<int>(levelData.size())) {
			return snapshot;
		}

		GameStats const& stats = levelData[levelId].GetGameStats(isPlayer);

		snapshot.shotsFired = stats.shotsFired;
		snapshot.shotsHit = stats.shotsHit;
		snapshot.shotsMiss = stats.shotsMiss;

		snapshot.totalAbilityUsed = stats.totalAbilityUsed;
		snapshot.offensiveAbilityUsed = stats.offensiveAbilityUsed;
		snapshot.defensiveAbilityUsed = stats.defensiveAbilityUsed;
		snapshot.statsAbilityUsed = stats.statsAbilityUsed;

		return snapshot;
	}

	CombatStatsSnapshot GameData::GetCurrentLevelStats(bool isPlayer) const {
		return GetLevelStats(currentLevelIdTracking, isPlayer);
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