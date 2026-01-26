#ifndef UTILS_HPP
#define UTILS_HPP

#include "pch.hpp"
#include "Color.hpp"

namespace Utils {
	extern bool isDebuggerOn;
	extern std::pair<Color, float> debuggerInfo; // Store the text color and text size
	typedef std::function<void()> FuncPtr;

	enum RectMode {
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

	std::string FormatFloat(double value, int precision);
	void Debugger(AEVec2 pos, std::string str);
	// Set limit to ensure that the value does not go under minimum or over maximum
	void Limit(int& val, int min, int max);
	// Set limit to ensure that the value does not go under minimum or over maximum
	void Limit(float& val, float min, float max);
	// Set limit to ensure that the value does not go under minimum or over maximum
	void Limit(double& val, double min, double max);
}

#endif // !UTILS_HPP
