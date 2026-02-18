#include "EnemyManager.hpp"

namespace EnemyManager {
	std::vector<Enemy>* enemies;

	void LoadEnemies(std::vector<Enemy>* list) {
		enemies = list;
	}
}