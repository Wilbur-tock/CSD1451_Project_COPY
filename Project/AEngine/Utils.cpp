#include "Utils.hpp"

#include "GraphicsManager.hpp"
#include "CameraManager.hpp"

namespace Utils {
	bool isDebuggerOn = false;
	bool isInstructionOn = false;
	bool isPaused = false;                                         // <-- NEW
	bool isSettingsOpen = false;                                    // settings overlay inside pause menu
	bool isResultScreenOn = false;                                  // defeat/victory screen active
	float unpauseCooldown = 0.0f;                                  // <-- NEW

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

	bool IsTileIndexOutOfBounds(int x, int y) {
		return x < 0 || x >= Config::GRID_WIDTH || y < 0 || y >= Config::GRID_HEIGHT;
	}

	void SetTileIndexWithinBounds(int& x, int& y) {
		x = x < Config::MIN_GRID_X ? Config::MIN_GRID_X : x;
		x = x > Config::MAX_GRID_X ? Config::MAX_GRID_X : x;
		y = y < Config::MIN_GRID_Y ? Config::MIN_GRID_Y : y;
		y = y > Config::MAX_GRID_Y ? Config::MAX_GRID_Y : y;
	}

	void CalculateTileBounds(TileIndex const& center, TileIndex& min, TileIndex& max, int radius) {
		// y is 0 at the top of the screen
		// min - bottom left corner of the box
		min.x = center.x - radius;
		min.y = center.y + radius;
		// max - top right corner of the box 
		max.x = center.x + radius;
		max.y = center.y - radius;

		// Ensure index is not out of bounds
		SetTileIndexWithinBounds(min.x, min.y);
		SetTileIndexWithinBounds(max.x, max.y);
	}

	void SetAnchoredPosition(Transform const& parent, Transform& trans, AnchorMode const& anchorMode) {
		// Set position based on screen width and height with its origin mode in the CENTER
		float newX = parent.pos.x, newY = parent.pos.y;
		float halfParentW = parent.size.x * 0.5f, halfParentH = parent.size.y * 0.5f;

		float chiildW = trans.size.x, childH = trans.size.y;
		float halfW = chiildW * 0.5f, halfH = childH * 0.5f;

		// Set position based on anchor mode
		switch (anchorMode) {
		case Utils::AnchorMode::TOP_LEFT:
			newX = -halfParentW;
			newY = halfParentH;
			break;
		case Utils::AnchorMode::TOP_CENTER:
			newX = -halfW;
			newY = halfParentH;
			break;
		case Utils::AnchorMode::TOP_RIGHT:
			newX = halfParentW - chiildW;
			newY = halfParentH;
			break;
		case Utils::AnchorMode::CENTER_LEFT:
			newX = -halfParentW;
			newY = halfH;
			break;
		case Utils::AnchorMode::CENTER:
			newX = -halfW;
			newY = halfH;
			break;
		case Utils::AnchorMode::CENTER_RIGHT:
			newX = halfParentW - chiildW;
			newY = halfH;
			break;
		case Utils::AnchorMode::BOTTOM_LEFT:
			newX = -halfParentW;
			newY = -halfParentH + childH;
			break;
		case Utils::AnchorMode::BOTTOM_CENTER:
			newX = -halfW;
			newY = -halfParentH + childH;
			break;
		case Utils::AnchorMode::BOTTOM_RIGHT:
			newX = halfParentW - chiildW;
			newY = -halfParentH + childH;
			break;
		}

		trans.pos.x = newX;
		trans.pos.y = newY;
	}

	void SetAnchoredPosition(Transform& trans, AnchorMode const& anchorMode) {
		SetAnchoredPosition(Transform{ {0,0}, {Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT} }, trans, anchorMode);
	}

	std::string FormatFloat(double value, int precision) {
		char buf[64];
		std::snprintf(buf, sizeof(buf), "%.*f", precision, value);
		return std::string(buf);
	}

	void WrapText(std::string& text, s8 font, float scale, float maxWidth) {
		float maxHeight = 0;
		Utils::NormalizeCoordinates(maxWidth, maxHeight);

		std::string result;
		std::istringstream lineStream(text);
		std::string line;

		while (std::getline(lineStream, line)) {
			std::stringstream ss(line);
			std::string word;
			std::string currentLine;

			while (ss >> word) {
				std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
				f32 testWidth, testHeight;
				AEGfxGetPrintSize(font, testLine.c_str(), scale, &testWidth, &testHeight);

				if (testWidth > maxWidth && !currentLine.empty()) {
					result += currentLine + "\n";
					currentLine = word;
				}
				else {
					currentLine = testLine;
				}
			}
			result += currentLine + "\n"; // Preserve the original newline
		}

		// Remove trailing newline
		if (!result.empty() && result.back() == '\n')
			result.pop_back();

		text = result;
	}
}