#include "Utils.hpp"

Color::Color(){}
Color::Color(float _r, float _g, float _b, float _a) {
	SetColor(_r, _g, _b, _a);
}

void Color::SetColor(float _r, float _g, float _b, float _a) {
	r = _r < min ? min : _r > max ? max : _r;
	g = _g < min ? min : _g > max ? max : _g;
	b = _b < min ? min : _b > max ? max : _b;
	a = _a < min ? min : _a > max ? max : _a;
}

float Color::GetRed() {
	return r;
}

float Color::GetGreen() {
	return g;
}

float Color::GetBlue() {
	return b;
}

float Color::GetAlpha() {
	return a;
}