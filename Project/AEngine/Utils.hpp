#ifndef UTILS_HPP
#define UTILS_HPP

#include "pch.hpp"

class Color {
private:
	float r, g, b, a;
	float min = 0.f, max = 1.f;
public:
	Color();
	Color(float _r, float _g, float _b, float _a = 1.f);
	void SetColor(float _r, float _g, float _b, float _a = 1.f);
	float GetRed();
	float GetGreen();
	float GetBlue();
	float GetAlpha();
};

#endif // !UTILS_HPP
