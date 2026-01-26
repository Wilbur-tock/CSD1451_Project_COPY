#ifndef UTILS_HPP
#define UTILS_HPP

#include "pch.hpp"

namespace Utils {
	extern bool isDebuggerOn;
	using FuncPtr = void(*)(void);

	// Get world mouse position instead of the screen position due to camera movement
	void GetWorldMousePos(float& x, float& y);
	void GetWorldPos(float& x, float& y);

	std::string FormatFloat(double value, int precision);
	// Set limit to ensure that the value does not go under minimum or over maximum
	void Limit(int& val, int min, int max);
	// Set limit to ensure that the value does not go under minimum or over maximum
	void Limit(float& val, float min, float max);
	// Set limit to ensure that the value does not go under minimum or over maximum
	void Limit(double& val, double min, double max);
}

#endif // !UTILS_HPP
