#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "pch.hpp"

namespace Config {
	constexpr int screenWidth = 1600, screenHeight = 900, tileSize = 32;
	constexpr int mapHeight = static_cast<int>(900 / tileSize) + 1, mapWidth = static_cast<int>(1600 / tileSize);
}

#define KEY_SAVEFILE AEVK_ESCAPE

#endif // !CONFIG_HPP
