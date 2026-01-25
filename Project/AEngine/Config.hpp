#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "pch.hpp"

namespace Config {
	constexpr int SCREEN_WIDTH = 1600, SCREEN_HEIGHT = 900, TILE_SIZE = 32;
	// World map
	const int widthMultiplier = 3, heightMultiplier = 2;
	constexpr float WORLD_MIN_X = -SCREEN_WIDTH* widthMultiplier / 2.0f, WORLD_MAX_X = SCREEN_WIDTH * widthMultiplier / 2.0f;
	constexpr float WORLD_MIN_Y = -SCREEN_HEIGHT * heightMultiplier / 2.0f, WORLD_MAX_Y = SCREEN_HEIGHT * heightMultiplier / 2.0f;

	constexpr int MAP_HEIGHT = static_cast<int>(900 * heightMultiplier / TILE_SIZE) + 1, MAP_WIDTH = static_cast<int>(SCREEN_WIDTH* widthMultiplier / TILE_SIZE);
	constexpr int MIN_GRID_X = 0, MAX_GRID_X = MAP_WIDTH - 1, MIN_GRID_Y = 0, MAX_GRID_Y = MAP_HEIGHT - 1;

	// Use font size to load font
	constexpr int FONT_SIZE = 24;
	// Use font scale to adjust the size of the text when it is being drawn
	constexpr float FONT_SCALE = 1.0f;
}

#define KEY_SAVEFILE AEVK_ESCAPE

#endif // !CONFIG_HPP
