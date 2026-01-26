#include "Button.hpp"

// string text, aevec2 _pos, width, float, color, texture,
Button::Button(std::string _text, AEVec2 _pos, float _width, float _height, Color _color, AEGfxTexture* _texture,
	float _textOffsetX, float _textOffsetY, Utils::FuncPtr _onClick)
	: text{ _text }, pos{ _pos }, width{ _width }, height{ _height }, color{ _color }, texture{ _texture },
	textOffsetX{ _textOffsetX }, textOffsetY{ _textOffsetY }, onClick{ _onClick } {
	// Add offset with the origin being top left corner of the screen
	//pos.x -= Config::SCREEN_WIDTH / 2.0f;
	//pos.y += (Config::SCREEN_HEIGHT / 2.0f);
}