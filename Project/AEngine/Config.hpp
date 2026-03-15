#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace Config {
	constexpr int SCREEN_WIDTH = 1600, SCREEN_HEIGHT = 900, TILE_SIZE = 32;
	// World map
	const int widthMultiplier = 3, heightMultiplier = 2;
	constexpr float WORLD_MIN_X = -SCREEN_WIDTH* widthMultiplier / 2.0f, WORLD_MAX_X = SCREEN_WIDTH * widthMultiplier / 2.0f;
	constexpr float WORLD_MIN_Y = -SCREEN_HEIGHT * heightMultiplier / 2.0f, WORLD_MAX_Y = SCREEN_HEIGHT * heightMultiplier / 2.0f;

	constexpr int GRID_HEIGHT = static_cast<int>(900 * heightMultiplier / TILE_SIZE) + 1, GRID_WIDTH = static_cast<int>(SCREEN_WIDTH* widthMultiplier / TILE_SIZE);
	constexpr int MIN_GRID_X = 0, MAX_GRID_X = GRID_WIDTH - 1, MIN_GRID_Y = 0, MAX_GRID_Y = GRID_HEIGHT - 1;

	// Use font size to load font
	constexpr int FONT_SIZE = 24;
	// Use font scale to adjust the size of the text when it is being drawn
	constexpr float FONT_SCALE = 1.0f;

	// CANNON
	constexpr float CANNON_MIN_POWER = 100, CANNON_MAX_POWER = 2500;
	constexpr float CANNON_SWITCH_COOLDOWN_TIMER = 1.5f, PROJECTILE_LIFESPAN = 2.0f;

	// Abilities
	constexpr int MAX_INVENTORY_COUNT = 5; // Maximum number of ability charges has in it inventory
	constexpr int MAX_ABILITY_SELECTOR_COUNT = 3; // Number of options to select ability at the start of each round
	constexpr int MAX_ABILITY_MODIFIER_COUNT = 5; // Maximum number of abilities effect that last for more than 1 turn player/enemy can have
}

#endif // !CONFIG_HPP
