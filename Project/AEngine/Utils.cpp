#include "Utils.hpp"

#include "GraphicsManager.hpp"

namespace Utils {
	bool isDebuggerOn = true;
	Color debuggerColor = Color{ 0,1.f,0 };

	std::pair<Color, float> debuggerInfo = {Color{0, 0.8f, 0 }, 0.75f};

	void GetWorldMousePos(float& x, float& y) {
		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		x = static_cast<float>(mouseX - (Config::SCREEN_WIDTH / 2.0f));
		y = static_cast<float>((Config::SCREEN_HEIGHT / 2.0f) - mouseY);

		GetWorldPos(x, y);
	}

	// values range from x = negative to positive max map width, y = 0 neagtive to positive max map height 
	void GetWorldPos(float& x, float& y) {
		f32 camX, camY;
		AEGfxGetCamPosition(&camX, &camY);

		// Adding the offset from camera moving and add the screenWidth if the mapWidth is larger than the screenWidth
		x += camX;
		y += camY;

		// Use if values is from 0 to max
		//x += camX + Config::SCREEN_WIDTH;
		//y += (Config::SCREEN_HEIGHT / 2.0f) - camY;
	}

	void Debugger(AEVec2 pos, std::string str) {
		GraphicsManager::DrawTextUI(str, -1.f, -0.9f, Color{0,1,0}, 0.8f);
	}

	std::string FormatFloat(double value, int precision) {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(precision) << value;
		return ss.str();
	}

	void Limit(int& val, int min, int max) {
		if (val < min) val = min;
		if (val > max) val = max;
	}
	
	void Limit(float& val, float min, float max) {
		if (val < min) val = min;
		if (val > max) val = max;
	}
	
	void Limit(double& val, double min, double max) {
		if (val < min) val = min;
		if (val > max) val = max;
	}

	void Wrap(int& val, int min, int max) {
		if (val < min) val = max;
		if (val > max) val = min;
	}

	void Wrap(float& val, float min, float max) {
		if (val < min) val = max;
		if (val > max) val = min;
	}

	void Wrap(double& val, double min, double max) {
		if (val < min) val = max;
		if (val > max) val = min;
	}
}