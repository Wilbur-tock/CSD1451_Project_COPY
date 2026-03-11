#include "pch.hpp"
#include "GameTrackingManager.hpp"

namespace GameTrackingManager {
	bool needToUpdateLog = true;
	GameTrackingData data;

	void GameStart() {
		data.sessionStart = AEGetTime(nullptr);
	}

	void GameEnd() {
		if (!needToUpdateLog) return;

		data.sessionDuration = AEGetTime(nullptr) - data.sessionStart;

		// Write data into a log.txt
		std::string filepath = "../../Assets/log.txt";
		std::ofstream output{ filepath };
		if (!output.is_open()) {
			std::cerr << "Error: Could not open file at " << filepath << std::endl;
			return;
		}

		int mins = static_cast<int>(data.sessionDuration) / 60;
		int secs = static_cast<int>(data.sessionDuration) % 60;

		std::streamsize w{ 30 };
		output << "=============== Code Tracking ==============="
			<< std::left << std::setw(w) << "\nDraw Mesh Count: " << data.meshDrawCount 
			<< "\n=============== Game Data ==============="
			<< std::left << std::setw(w) << "\nGame Duration: " << mins << " mins " << secs << " secs"
			<< std::left << std::setw(w) << "\nPlayer shots: " << data.playerShotsFired
			<< std::left << std::setw(w) << "\nPlayer hits: " << data.playerShotsHit
			<< std::left << std::setw(w) << "\nPlayer miss: " << data.playerShotsMiss
			<< std::left << std::setw(w) << "\nEnemy shots: " << data.enemyShotsFired
			<< std::left << std::setw(w) << "\nEnemy hits: " << data.enemyShotsHit
			<< std::left << std::setw(w) << "\nEnemy miss: " << data.enemyShotsMiss
			<< std::endl;
		output.close();
	}
}