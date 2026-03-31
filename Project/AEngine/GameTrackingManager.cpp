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

		// Save data into a file
		if (needToUpdateLog) {
			std::string filepath = "Assets/Data/gameLog.txt";

			std::ofstream ofs{filepath};
			if (ofs.is_open()) {
				WriteHeader(ofs, "All game data");
				WriteData(ofs, globalData);

				for (int i = 0; i < static_cast<int>(levelData.size()); ++i) {
					WriteHeader(ofs, "Level " + std::to_string(i) + " data");
					WriteData(ofs, levelData[i]);
				}
			}
		}
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

		globalStats.shotsAccuracy = Utils::GetPercentage(globalStats.shotsHit, globalStats.shotsFired);
		levelStats.shotsAccuracy = Utils::GetPercentage(levelStats.shotsHit, levelStats.shotsFired);
	}

	void GameData::AddDamage(bool isPlayer, bool isTiles, int damage) {
		GameStats& globalStats = globalData.GetGameStats(isPlayer);
		GameStats& levelStats = levelData[currentLevelIdTracking].GetGameStats(isPlayer);

		globalStats.totalDamage += damage;
		levelStats.totalDamage += damage;

		if (isTiles) {
			globalStats.tilesDamage += damage;
			levelStats.tilesDamage += damage;
		}
		else {
			globalStats.characterDamage += damage;
			levelStats.characterDamage += damage;
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

	GameStats const& GameData::GetLevelStats(int levelId, bool isPlayer) {
		if (levelId < 0 || levelId >= levelData.size()) {
			levelId = 0;
		}

		return levelData[levelId].GetGameStats(isPlayer);
	}

	GameStats const& GameData::GetCurrentLevelStats(bool isPlayer) {
		return levelData[currentLevelIdTracking].GetGameStats(isPlayer);
	}

	void GameData::WriteHeader(std::ofstream& ofs, std::string title) {
		size_t stringSize = 75 - title.size();

		std::string border(stringSize/2, '=');
		ofs << border << title << border;

		if (stringSize % 2 != 0) ofs << '=';
	}

	void GameData::WriteData(std::ofstream& ofs, GameSession gameSession) {
		int mins = static_cast<int>(gameSession.sessionDuration) / 60;
		int secs = static_cast<int>(gameSession.sessionDuration) % 60;

		// Print session data
		ofs << std::left << std::setw(width) << "\nGame Duration: " << mins << " mins " << secs << " secs" 
			<< std::left << std::setw(width) << "\nTotal Rounds Played: " << gameSession.rounds
			<< std::endl;

		WriteHeader(ofs, "Player Data");
		WriteGameStats(ofs, gameSession.GetGameStats(true));
		WriteHeader(ofs, "Enemy Data");
		WriteGameStats(ofs, gameSession.GetGameStats(false));
	}

	void GameData::WriteGameStats(std::ofstream& ofs, GameStats stats) {
		ofs << std::left << std::setw(width) << "\nTotal Shots: " << stats.shotsFired
			<< std::left << std::setw(width) << "\nHits: " << stats.shotsHit
			<< std::left << std::setw(width) << "\nMiss: " << stats.shotsMiss 
			<< std::left << std::setw(width) << "\nAccuracy: " << stats.shotsAccuracy << "%\n"
			<< std::left << std::setw(width) << "\nTotal Damage: " << stats.totalDamage
			<< std::left << std::setw(width) << "\nTotal Tiles Damage: " << stats.tilesDamage
			<< std::left << std::setw(width) << "\nTotal Character Damage: " << stats.characterDamage << '\n'
			<< std::left << std::setw(width) << "\nTotal Ability Used: " << stats.totalAbilityUsed
			<< std::left << std::setw(width) << "\nStats Ability Used: " << stats.statsAbilityUsed
			<< std::left << std::setw(width) << "\nOffensive Ability Used: " << stats.offensiveAbilityUsed
			<< std::left << std::setw(width) << "\nDefensive Ability Used: " << stats.defensiveAbilityUsed
			<< '\n' << std::endl;
	}
}