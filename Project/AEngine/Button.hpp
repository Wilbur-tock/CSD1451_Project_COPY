#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "pch.hpp"

#include "Color.hpp"
#include "Utils.hpp"

class Button {
public:
	std::string text = "";
	AEVec2 pos{0,0};
	float width = 0.0f, height = 0.0f;
	float textOffsetX = 0.0f, textOffsetY = 0.0f; // Use range 0.0 - 0.1f to offset the text position according to the button
	Color color{1,1,1};
	AEGfxTexture* texture = nullptr;
	Utils::FuncPtr onClick = nullptr;

	Button() = default;
	Button(std::string _text, AEVec2 _pos, float _width, float _height, Color _color, AEGfxTexture* _texture,
		Utils::RectMode _rectMode, Utils::FuncPtr _onClick = nullptr);
};

#endif // !BUTTON_HPP
