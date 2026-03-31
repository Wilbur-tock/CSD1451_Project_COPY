/*
Authors: Dayrius
Description: Track game data throughout the whole game and save the data to a file

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#pragma once
#ifndef GAMETRACKINGAMANAGER_HPP
#define GAMETRACKINGAMANAGER_HPP

#include "pch.hpp"

namespace GameTrackingManager {
	struct GameStats {
		size_t shotsFired = 0;
		size_t shotsHit = 0;
		size_t shotsMiss = 0;

		float shotsAccuracy = 0.0f;

		size_t totalDamage = 0;
		size_t tilesDamage = 0;
		size_t characterDamage = 0;

		size_t totalAbilityUsed = 0;
		size_t offensiveAbilityUsed = 0;
		size_t defensiveAbilityUsed = 0;
		size_t statsAbilityUsed = 0;
	};

	// track each session overall and each levels data
	class GameSession {
	public:
		GameStats& GetGameStats(bool isPlayer) { return isPlayer ? player : enemy; }
		GameStats const& GetGameStats(bool isPlayer) const { return isPlayer ? player : enemy; }

		int rounds = 0;
		double sessionStart = 0;    // store start time
		double sessionDuration = 0; // calculated at log write
	private:
		GameStats player;
		GameStats enemy;
	};

	class GameData {
	public:
		void GameStart();
		void GameEnd();

		void LevelStart(int levelId);
		void LevelEnd(int levelId);

		void AddShotFired(bool isPlayer);
		void AddShots(bool isPlayer, bool isHit);
		void AddDamage(bool isPlayer, bool isTiles, int damage);
		void AddAbilityUsed(bool isPlayer, int typeId);

		GameStats const& GetLevelStats(int levelId, bool isPlayer);
		GameStats const& GetCurrentLevelStats(bool isPlayer);

	private:
		void WriteHeader(std::ofstream& ofs, std::string title);
		void WriteData(std::ofstream& ofs, GameSession gameData);
		void WriteGameStats(std::ofstream& ofs, GameStats stats);

	private:
		GameSession globalData;
		std::vector<GameSession> levelData;

		bool needToUpdateLog = true;

		int currentLevelIdTracking = 0;
		std::streamsize width{ 50 };
	};

	extern GameData gameData;
}

#endif // !DIAGNOSTICSMANAGER_HPP
