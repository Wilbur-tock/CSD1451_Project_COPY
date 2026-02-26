#include "Utils.hpp"

#include "GraphicsManager.hpp"
#include "CameraManager.hpp"

namespace Utils {
	bool isDebuggerOn = true;
	Color debuggerColor = Color{ 0,1.f,0 };

	std::pair<float, Color> debuggerInfo = { 0.75f, Color{0, 0.8f, 0 }};

	void GetWorldMousePos(float& x, float& y) {
		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		x = static_cast<float>(mouseX - (Config::SCREEN_WIDTH / 2.0f));
		y = static_cast<float>((Config::SCREEN_HEIGHT / 2.0f) - mouseY);

		// Divide by zoom so screen coords map to correct world coords
		float zoom = CameraManager::camera.GetZoom();
		x /= zoom;
		y /= zoom;

		GetWorldPos(x, y);
	}

	// values range from x = negative to positive max map width, y = 0 neagtive to positive max map height 
	void GetWorldPos(float& x, float& y) {
		f32 camX, camY;
		AEGfxGetCamPosition(&camX, &camY);

		// Adding the offset from camera moving and add the screenWidth if the mapWidth is larger than the screenWidth
		x += camX;
		y += camY;
	}

	void NormalizeCoordinates(float& x, float& y) {
		x /= (Config::SCREEN_WIDTH / 2.0f);
		y /= (Config::SCREEN_HEIGHT / 2.0f);
	}

	void GetMouseGridIndex(int& x, int& y) {
		// Calculate Grid x and y index
		float worldMouseX = 0, worldMouseY = 0;
		Utils::GetWorldMousePos(worldMouseX, worldMouseY);

		GetGridIndex(x, y, { worldMouseX, worldMouseY });

		// This is to ensure that the grid index does not go out of range
		x = std::clamp(x, Config::MIN_GRID_X, Config::MAX_GRID_X);
		y = std::clamp(y, Config::MIN_GRID_Y, Config::MAX_GRID_Y);
	}

	void GetGridIndex(int& x, int& y, AEVec2 const worldPos) {
		x = static_cast<int>((worldPos.x + Config::WORLD_MAX_X) / Config::TILE_SIZE);
		y = static_cast<int>((Config::WORLD_MAX_Y - worldPos.y) / Config::TILE_SIZE);
	}

	bool IsGridIndexOutOfBounds(int x, int y) {
		return x < 0 || x >= Config::GRID_WIDTH || y < 0 || y >= Config::GRID_HEIGHT;
	}

	std::string FormatFloat(double value, int precision) {
		char buf[64];
		std::snprintf(buf, sizeof(buf), "%.*f", precision, value);
		return std::string(buf);
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