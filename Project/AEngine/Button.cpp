#include "Button.hpp"

// string text, aevec2 _pos, width, float, color, texture,
Button::Button(std::string _text, AEVec2 _pos, float _width, float _height, Color _color, AEGfxTexture* _texture, Utils::RectMode _rectMode, Utils::FuncPtr _onClick)
	: text{ _text }, pos{ _pos }, width{ _width }, height{ _height }, color{ _color }, texture{ _texture },
	 onClick{ _onClick } {
	// Add offset for the text inside the button
	//float gap = 0.01f, x = 0, y = 0;
    //float halfW = _width / 2.0f;
    //float halfH = _height / 2.0f;
    // NOT FIXED YET TO BE FIXED
    //switch (_rectMode) {
    //case Utils::RECT_TOP_LEFT:
    //    x = 0;          
    //    y = 0;
    //    break;
    //case Utils::RECT_TOP_CENTER:
    //    x = halfW;      y = 0;
    //    break;
    //case Utils::RECT_TOP_RIGHT:
    //    x = _width;     y = 0;
    //    break;
    //case Utils::RECT_CENTER_LEFT:
    //    x = 0;          y = -halfH;
    //    break;
    //case Utils::RECT_CENTER:
    //    x = halfW;      y = -halfH;
    //    break;
    //case Utils::RECT_CENTER_RIGHT:
    //    x = _width;     y = -halfH;
    //    break;
    //case Utils::RECT_BOTTOM_LEFT:
    //    x = 0;          y = -_height;
    //    break;
    //case Utils::RECT_BOTTOM_CENTER:
    //    x = halfW;      y = -_height;
    //    break;
    //case Utils::RECT_BOTTOM_RIGHT:
    //    x = _width;     y = -_height;
    //    break;
    //}

	textOffsetX = 0;
	textOffsetY = 0;
}