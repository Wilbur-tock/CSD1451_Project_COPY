#ifndef PLAYERMANAGER_HPP
#define PLAYERMANAGER_HPP

#include "pch.hpp"

#include "AbilitiesManager.hpp"
#include "Character.hpp"

namespace PlayerManager {
	extern AbilitiesManager::AbilityController playerAbilityController;
	extern std::vector<Character> playerCharacters;
}

#endif // !PLAYERMANAGER_HPP
