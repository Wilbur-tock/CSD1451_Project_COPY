#ifndef ENEMYMANAGER_HPP
#define ENEMYMANAGER_HPP

#include "pch.hpp"
#include "Enemy.hpp"

namespace EnemyManager {
	extern std::vector<Enemy>* enemies;

	void LoadEnemies(std::vector<Enemy>* list);
}

#endif // !1