#pragma once
#ifndef GAMETRACKINGAMANAGER_HPP
#define GAMETRACKINGAMANAGER_HPP

#include "pch.hpp"

namespace GameTrackingManager {
	class GameData {
	private:
		struct GameStats {
			size_t shotsFired = 0;
			size_t shotsHit = 0;
			size_t shotsMiss = 0;

			size_t totalAbilityUsed = 0;
			size_t offensiveAbilityUsed = 0;
			size_t defensiveAbilityUsed = 0;
			size_t statsAbilityUsed = 0;
		};

		class GameSession {
		public:
			GameStats& GetGameStats(bool isPlayer) { return isPlayer ? player : enemy; }

			int rounds = 0;
			double sessionStart = 0;    // store start time
			double sessionDuration = 0; // calculated at log write
		private:
			GameStats player;
			GameStats enemy;
		};

	public:
		void GameStart();
		void GameEnd();

		void LevelStart(int levelId);
		void LevelEnd(int levelId);

		void AddShotFired(bool isPlayer);
		void AddShots(bool isPlayer, bool isHit);
		void AddAbilityUsed(bool isPlayer, int typeId);

	private:
		void PrintHeader(std::ofstream& ofs, std::string title);
		void PrintData(std::ofstream& ofs, GameSession gameData);
		void PrintGameStats(std::ofstream& ofs, GameStats stats);

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
