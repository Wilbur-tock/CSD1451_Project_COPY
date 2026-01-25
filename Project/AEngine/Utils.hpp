#ifndef UTILS_HPP
#define UTILS_HPP

#include "pch.hpp"

namespace Utils {
	using FuncPtr = void(*)(void);

	// Get world mouse position instead of the screen position due to camera movement
	void GetWorldMousePos(float& x, float& y);
	void GetWorldPos(float& x, float& y);

	std::string FormatFloat(float value, int precision);
	// Set limit to ensure that the value does not go under minimum or over maximum
	void Limit(int& val, int min, int max);
}

#endif // !UTILS_HPP
