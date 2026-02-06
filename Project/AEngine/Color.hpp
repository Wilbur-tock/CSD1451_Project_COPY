#ifndef COLOR_HPP
#define COLOR_HPP

#include "pch.hpp"

class Color {
private:
	float r, g, b, a;
	float min = 0.f, max = 1.f;
public:
	Color();
	Color(float _r, float _g, float _b, float _a = 1.f);
	void SetColor(float _r, float _g, float _b, float _a = 1.f);
	float GetRed() const;
	float GetGreen() const;
	float GetBlue() const;
	float GetAlpha() const;
	void SetAlpha(float _a);
};

#endif // ! COLOR_HPP
