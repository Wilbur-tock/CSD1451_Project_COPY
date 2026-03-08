#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "pch.hpp"

#include "Color.hpp"
#include "Utils.hpp"
#include "TextObject.hpp"

class Button {
public:
	Button() = default;
	Button(AEVec2 _pos, AEVec2 _size, Color _color, AEGfxTexture* _texture, Utils::FuncPtr _onClick = nullptr);
	// Create a button with text
	// textObj - contains the text content to be displayed onto the button 
	// with its default position being 0,0 (Top left corner of the button object)
	// _rectMode - set where the text content will be print out 
	// e.g. RECT_CENTER will print text to align center of the button
	Button(TextObject _textObj, Utils::AnchorMode _anchorMode, AEVec2 _pos, AEVec2 _size, Color _color,
		AEGfxTexture* _texture, Utils::FuncPtr _onClick = nullptr);

	// Check for player hover and whatever or not it is clicked
	void Update();
	void Draw();

	void SetTextOffset(f32 offsetX, f32 offsetY);

private:
	TextObject textObj;
	AEVec2 pos{ 0,0 }, size{ 0,0 };
	Color color{ 1,1,1 };
	AEGfxTexture* texture = nullptr;
	Utils::FuncPtr onClick = nullptr;
	bool isHovered = false;
};

#endif // !BUTTON_HPP
