#ifndef UTILS_HPP
#define UTILS_HPP

#include "pch.hpp"
#include "Color.hpp"
#include "BaseTypes.hpp"
#include "TileIndex.hpp"

namespace Utils {
	extern bool isDebuggerOn;
	extern bool isInstructionOn;
	extern bool isPaused;                                          // <-- NEW (pause menu)
	extern bool isSettingsOpen;                                     // settings overlay inside pause menu
	extern bool isResultScreenOn;                                   // defeat/victory screen active
	extern float unpauseCooldown;                                  // <-- NEW (input delay after resume)
	extern std::pair<float, Color> debuggerInfo; // Store the text scale and color
	typedef std::function<void()> FuncPtr;

	enum class AnchorMode {
		TOP_LEFT,
		TOP_CENTER,
		TOP_RIGHT,
		CENTER_LEFT,
		CENTER,
		CENTER_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		BOTTOM_RIGHT,
	};

	// Get world mouse position instead of the screen position due to camera movement
	void GetWorldMousePos(float& x, float& y);
	// Set world position according to the camera position
	// values range from x = negative to positive max map width, y = 0 neagtive to positive max map height 
	void GetWorldPos(float& x, float& y);
	void NormalizeCoordinates(float& x, float& y);
	AEVec2 NormalizeCoordinates(AEVec2 vec2);
	// Get the grid index based on mouse position
	void GetMouseGridIndex(int& x, int& y);
	void GetTileIndex(int& x, int& y, AEVec2 const worldPos);
	// Return true if it is out of bounds
	bool IsTileIndexOutOfBounds(int x, int y);
	// Check if tile index is out of bounds. if out of bounds, set it to be within bounds
	void SetTileIndexWithinBounds(int& x, int& y);

	void CalculateTileBounds(TileIndex const& center, TileIndex& min, TileIndex& max, int radius);

	void SetAnchoredPosition(Transform const& parent, Transform& trans, AnchorMode const& anchorMode);
	// Set anchoredPosition with parent being the screen transform (width and height)
	void SetAnchoredPosition(Transform& trans, AnchorMode const& anchorMode);

	std::string FormatFloat(double value, int precision);

	template <typename T>
	T RandomValue(T min, T max) {
		return static_cast<T>(min + (AERandFloat() * (max - min)));
	}

	// Set val to wrap to min value if it is MORE THAN max and vice versa
	template <typename T>
	void Wrap(T& val, T min, T max) {
		if (val < min) val = max;
		if (val > max) val = min;
	}
}

#endif // !UTILS_HPP