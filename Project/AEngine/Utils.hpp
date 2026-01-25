#ifndef UTILS_HPP
#define UTILS_HPP

#include "pch.hpp"

using FP = void(*)(void);

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

class Button {
public:
	std::string text;
	AEVec2 pos;
	float width, height;
	float textOffsetX, textOffsetY; // Use range 0.0 - 0.1f to offset the text position according to the button
	Color color;
	AEGfxTexture* texture;
	FP onClick;

	Button() = default;
	Button(std::string _text, AEVec2 _pos, float _width, float _height, Color _color, AEGfxTexture* _texture, 
		float _textOffsetX = 0.f, float _textOffsetY = 0.f, FP _onClick = nullptr);
};

namespace Utils {
	// Get world mouse position instead of the screen position due to camera movement
	void GetWorldMousePos(float& x, float& y);
	void GetWorldPos(float& x, float& y);

	std::string FormatFloat(float value, int precision);
	void Limit(int& val, int min, int max);
}

#endif // !UTILS_HPP
