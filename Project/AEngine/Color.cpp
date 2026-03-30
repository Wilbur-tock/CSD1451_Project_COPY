/*
Authors: Dayrius
Description: Stores data and functions about colors

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/


#include "Color.hpp"

Color::Color() {
	SetColor(0, 0, 0, 0);
}
Color::Color(float _r, float _g, float _b, float _a) {
	SetColor(_r, _g, _b, _a);
}

void Color::SetColor(float _r, float _g, float _b, float _a) {
	r = _r < min ? min : _r > max ? max : _r;
	g = _g < min ? min : _g > max ? max : _g;
	b = _b < min ? min : _b > max ? max : _b;
	a = _a < min ? min : _a > max ? max : _a;
}

void Color::SetColor(Color const& _color) {
	SetColor(_color.Red(), _color.Green(), _color.Blue());
}

void Color::EditColorPercentage(float percentage) {
	SetColor(
		r *= percentage,
		g *= percentage,
		b *= percentage
	);
}

Color playerHPColor	{ 0.0f,0.9f,0.0f };
Color enemyHPColor	{ 0.9f,0.0f,0.0f };
Color shieldColor	{ 0.9f,0.9f,0.0f };
Color statsBGColor	{ 0.75f, 0.75f, 0.75f };

Color const& PlayerHPColor()	{ return playerHPColor; }
Color const& EnemyHPColor()		{ return enemyHPColor; }
Color const& ShieldColor()		{ return shieldColor; }
Color const& PanelColor()		{ return statsBGColor; }