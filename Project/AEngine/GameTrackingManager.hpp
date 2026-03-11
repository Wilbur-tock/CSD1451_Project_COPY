#pragma once
#ifndef GAMETRACKINGAMANAGER_HPP
#define GAMETRACKINGAMANAGER_HPP

namespace GameTrackingManager {
	struct GameTrackingData {
		size_t meshDrawCount = 0;

		size_t playerShotsFired = 0;
		size_t playerShotsHit = 0;
		size_t playerShotsMiss = 0;
		size_t enemyShotsFired = 0;
		size_t enemyShotsHit = 0;
		size_t enemyShotsMiss = 0;

		double sessionStart = 0;    // store start time
		double sessionDuration = 0; // calculated at log write
	};

	extern GameTrackingData data;

	void GameStart();
	void GameEnd();
}

#endif // !DIAGNOSTICSMANAGER_HPP
