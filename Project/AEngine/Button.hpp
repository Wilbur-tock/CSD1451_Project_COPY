#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "pch.hpp"

#include "Color.hpp"
#include "Utils.hpp"

class Button {
public:
	std::string text;
	AEVec2 pos;
	float width, height;
	float textOffsetX, textOffsetY; // Use range 0.0 - 0.1f to offset the text position according to the button
	Color color;
	AEGfxTexture* texture;
	Utils::FuncPtr onClick;

	Button() = default;
	Button(std::string _text, AEVec2 _pos, float _width, float _height, Color _color, AEGfxTexture* _texture,
		Utils::RectMode _rectMode, Utils::FuncPtr _onClick = nullptr);
};

#endif // !BUTTON_HPP
