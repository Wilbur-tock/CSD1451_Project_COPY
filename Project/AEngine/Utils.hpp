#ifndef UTILS_HPP
#define UTILS_HPP

#include "pch.hpp"
#include "Color.hpp"

namespace Utils {
	extern bool isDebuggerOn;
	extern std::pair<float, Color> debuggerInfo; // Store the text scale and color
	typedef std::function<void()> FuncPtr;

	enum class RectMode {
		RECT_TOP_LEFT,
		RECT_TOP_CENTER,
		RECT_TOP_RIGHT,
		RECT_CENTER_LEFT,
		RECT_CENTER,
		RECT_CENTER_RIGHT,
		RECT_BOTTOM_LEFT,
		RECT_BOTTOM_CENTER,
		RECT_BOTTOM_RIGHT,
	};

	// Get world mouse position instead of the screen position due to camera movement
	void GetWorldMousePos(float& x, float& y);
	void GetWorldPos(float& x, float& y);
	void NormalizeCoordinates(float& x, float& y);
	AEVec2 NormalizeCoordinates(AEVec2 vec2);
	// Get the grid index based on mouse position
	void GetMouseGridIndex(int& x, int& y);
	void GetGridIndex(int& x, int& y, AEVec2 const worldPos);
	// Return true if it is out of bounds
	bool IsGridIndexOutOfBounds(int x, int y);

	std::string FormatFloat(double value, int precision);

	template <typename T>
	// Set val to wrap to max if it go below min and vice versa
	void Wrap(T& val, T min, T max) {
		if (val < min) val = max;
		if (val > max) val = min;
	}
}

#endif // !UTILS_HPP
