#ifndef UTILS_HPP
#define UTILS_HPP

#include "pch.hpp"
#include "Color.hpp"

namespace Utils {
	extern bool isDebuggerOn;
	extern std::pair<float, Color> debuggerInfo; // Store the text scale and color
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
	void ToNormalize(float& x, float& y);
	// Get the grid index based on mouse position
	void GetGridIndex(int& x, int& y);

	std::string FormatFloat(double value, int precision);
	// Set limit to ensure that the value does not go under minimum or over maximum
	void Limit(int& val, int min, int max);
	// Set limit to ensure that the value does not go under minimum or over maximum
	void Limit(float& val, float min, float max);
	// Set limit to ensure that the value does not go under minimum or over maximum
	void Limit(double& val, double min, double max);
	// Set val to wrap to max if it go below min and vice versa
	void Wrap(int& val, int min, int max);
	// Set val to wrap to max if it go below min and vice versa
	void Wrap(float& val, float min, float max);
	// Set val to wrap to max if it go below min and vice versa
	void Wrap(double& val, double min, double max);
}

#endif // !UTILS_HPP
