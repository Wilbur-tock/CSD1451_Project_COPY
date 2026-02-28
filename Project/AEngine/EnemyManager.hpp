#ifndef ENEMYMANAGER_HPP
#define ENEMYMANAGER_HPP

#include "pch.hpp"
#include "Enemy.hpp"

namespace EnemyManager {
	extern std::vector<Enemy>* enemies;

	class EnemyController {
	public:
	private:
		bool isEnemyTurn = false;
	};
}

#endif // !1