#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "pch.hpp"

#include "BaseTypes.hpp"
#include "Color.hpp"
#include "Utils.hpp"
#include "TextObject.hpp"

class Button {
public:
	Button() = default;
	// Create a image button without any text or with an icon only
	Button(AEVec2 _pos, AEVec2 _size, Color _color, AEGfxTexture* _bgTexture, AEGfxTexture* _iconTexture = nullptr, Utils::FuncPtr _onClick = nullptr);
	// Create a button with text
	// textObj - contains the text content to be displayed onto the button 
	// with its default position being 0,0 (Top left corner of the button object)
	// _rectMode - set where the text content will be print out 
	// e.g. RECT_CENTER will print text to align center of the button
	Button(TextObject _textObj, Utils::AnchorMode _anchorMode, AEVec2 _pos, AEVec2 _size, Color _color,
		AEGfxTexture* _bgTexture = nullptr, AEGfxTexture* _iconTexture = nullptr, Utils::FuncPtr _onClick = nullptr);

	bool IsClick() const { return isClick; }
	bool IsHovered() const { return isHovered; }
	Transform GetTransform() const { return transform; }

	void SetOnClick(Utils::FuncPtr _OnClick) { onClick = _OnClick; }
	void SetOnHovered(Utils::FuncPtr _onHovered) { onHovered = _onHovered; }

	TextObject& GetTextObject() { return textObj; }
	void SetColor(Color _color) { color = _color; }

	// Check for player hover and whatever or not it is clicked
	void Update();
	void Draw();

	void SetTextOffset(float offsetX, float offsetY);
	// Change the anchorPosition of the button based on the screen transform and anchorMode assigned
	void SetPosition(Transform const& parent, Utils::AnchorMode mode);
	void AddOffsetPosition(AEVec2 const& _pos) { transform.pos.x += _pos.x, transform.pos.y += _pos.y; }

	void SetIcon(AEGfxTexture* newTexture) {
		iconTexture = newTexture;
	}
	void SetIconGaps(BorderGaps _borderGaps);
private:
	TextObject textObj;
	Transform transform = { {0,0}, {0,0} };
	Color color{ 1,1,1 };
	AEGfxTexture* bgTexture = nullptr;
	AEGfxTexture* iconTexture = nullptr;

	// Functions to calls
	Utils::FuncPtr onHovered = nullptr;
	Utils::FuncPtr onClick = nullptr;

	bool isHovered = false;
	bool isClick = false;

	Transform iconTransform = { {0,0}, {0,0} };
	BorderGaps borderGaps{ 5,5,5,5 }; // icon gap between x and y button bg
};

#endif // !BUTTON_HPP
