#ifndef ENEMYMANAGER_HPP
#define ENEMYMANAGER_HPP

#include "pch.hpp"
#include "Character.hpp"

namespace EnemyManager {
	extern std::vector<Character>* enemies;

	class EnemyController {
	public:
	private:
		bool isEnemyTurn = false;
	};
}

#endif // !1