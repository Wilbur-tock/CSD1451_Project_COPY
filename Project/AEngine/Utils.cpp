#include "Utils.hpp"

namespace Utils {
	void GetWorldMousePos(float& x, float& y) {
		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		GetWorldPos(x, y);
		x += mouseX;
		y += mouseY;
	}

	// values range from x = 0 to max map width, y = 0 to max map height 
	void GetWorldPos(float& x, float& y) {
		f32 camX, camY;
		AEGfxGetCamPosition(&camX, &camY);

		// Adding the offset from camera moving and add the screenWidth if the mapWidth is larger than the screenWidth
		x += camX + Config::SCREEN_WIDTH;
		y += (Config::SCREEN_HEIGHT / 2.0f) - camY;
	}

	std::string FormatFloat(float value, int precision) {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(precision) << value;
		return ss.str();
	}

	void Limit(int& val, int min, int max) {
		if (val < min) val = min;
		if (val > max) val = max;
	}
}