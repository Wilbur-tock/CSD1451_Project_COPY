#include "Utils.hpp"

#include "GraphicsManager.hpp"
#include "CameraManager.hpp"
#include "BaseTypes.hpp"

namespace Utils {
	bool isDebuggerOn = true;
	bool isInstructionOn = false;
	bool isPaused = false;                                         // <-- NEW
	bool isSettingsOpen = false;                                    // settings overlay inside pause menu
	bool isResultScreenOn = false;                                  // defeat/victory screen active
	float unpauseCooldown = 0.0f;                                  // <-- NEW
	Color debuggerColor = Color{ 0,1.f,0 };

	std::pair<float, Color> debuggerInfo = { 0.75f, Color{0, 0.8f, 0 } };

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

	AEVec2 NormalizeCoordinates(AEVec2 vec2) {
		NormalizeCoordinates(vec2.x, vec2.y);
		return vec2;
	}

	void GetMouseGridIndex(int& x, int& y) {
		// Calculate Grid x and y index
		float worldMouseX = 0, worldMouseY = 0;
		Utils::GetWorldMousePos(worldMouseX, worldMouseY);

		GetTileIndex(x, y, { worldMouseX, worldMouseY });

		// This is to ensure that the grid index does not go out of range
		x = std::clamp(x, Config::MIN_GRID_X, Config::MAX_GRID_X);
		y = std::clamp(y, Config::MIN_GRID_Y, Config::MAX_GRID_Y);
	}

	void GetTileIndex(int& x, int& y, AEVec2 const worldPos) {
		x = static_cast<int>((worldPos.x + Config::WORLD_MAX_X) / Config::TILE_SIZE);
		y = static_cast<int>((Config::WORLD_MAX_Y - worldPos.y) / Config::TILE_SIZE);
	}

	bool IsGridIndexOutOfBounds(int x, int y) {
		return x < 0 || x >= Config::GRID_WIDTH || y < 0 || y >= Config::GRID_HEIGHT;
	}

	void SetAnchoredPosition(Transform const& parent, Transform& trans, AnchorMode const& anchorMode) {
		// Set position based on screen width and height with its origin mode in the CENTER
		float parentX = parent.pos.x, parentY = parent.pos.y;
		float halfParentW = parent.size.x * 0.5f, halfParentH = parent.size.y * 0.5f;

		float chiildW = trans.size.x, childH = trans.size.y;
		float halfW = chiildW * 0.5f, halfH = childH * 0.5f;

		// Set position based on anchor mode
		switch (anchorMode) {
		case Utils::AnchorMode::TOP_LEFT:
			parentX = -halfParentW;
			parentY = halfParentH;
			break;
		case Utils::AnchorMode::TOP_CENTER:
			parentX = -halfW;
			parentY = halfParentH;
			break;
		case Utils::AnchorMode::TOP_RIGHT:
			parentX = halfParentW - chiildW;
			parentY = halfParentH;
			break;
		case Utils::AnchorMode::CENTER_LEFT:
			parentX = -halfParentW;
			parentY = halfH;
			break;
		case Utils::AnchorMode::CENTER:
			parentX = -halfW;
			parentY = halfH;
			break;
		case Utils::AnchorMode::CENTER_RIGHT:
			parentX = halfParentW - chiildW;
			parentY = halfH;
			break;
		case Utils::AnchorMode::BOTTOM_LEFT:
			parentX = -halfParentW;
			parentY = -halfParentH + childH;
			break;
		case Utils::AnchorMode::BOTTOM_CENTER:
			parentX = -halfW;
			parentY = -halfParentH + childH;
			break;
		case Utils::AnchorMode::BOTTOM_RIGHT:
			parentX = halfParentW - chiildW;
			parentY = -halfParentH + childH;
			break;
		}

		trans.pos.x = parentX;
		trans.pos.y = parentY;
	}

	void SetAnchoredPosition(Transform& trans, AnchorMode const& anchorMode) {
		SetAnchoredPosition(Transform{ {0,0}, {Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT} }, trans, anchorMode);
	}

	std::string FormatFloat(double value, int precision) {
		char buf[64];
		std::snprintf(buf, sizeof(buf), "%.*f", precision, value);
		return std::string(buf);
	}
}