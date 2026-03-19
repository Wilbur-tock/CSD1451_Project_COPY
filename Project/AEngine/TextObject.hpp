#ifndef TEXTOBJECT_HPP
#define TEXTOBJECT_HPP

#include "pch.hpp"

#include "Color.hpp"
#include "Utils.hpp"
#include "BaseTypes.hpp"

class TextObject {
public:
	TextObject();
	/*!***********************************************************************
	\brief
	To draw text UI object using noramlized coordinates
	\param[in] std::string _text 
	- text content to be displayed
	\param[in] _scale
	- use range from 0.0 to 1.0f
	\param[in] color
	- color of the text to be displayed
	\param[in] pos
	- use NORMALIZED COORDINATES range from -1.0f to 1.0f
	*************************************************************************/
	TextObject(std::string _text, float _scale = 1.0f, Color _color = Color{ 1,1,1 }, Transform _transform = { {0,0}, {0,0} }, Utils::AnchorMode _anchorMode = Utils::AnchorMode::TOP_LEFT);

	// Set a new anchorMode and set a new parent transform to update the position of this textObject (Called in Init)
	void SetPosition(Utils::AnchorMode newMode, Transform const& parentTransform);
	// Use the current anchorMode and set a new parent transform to update the position of this textObject (Called in Init)
	void SetPosition(Transform const& parentTransform);
	// Set the offset position to be used to offset from the anchor point that was set (Called in Init)
	void SetOffsetPosition(float x, float y);

	Transform& GetTransform() { return transform; }

	void TextScale(float newScale) { textScale = newScale; }
	void SetColor(Color const& newColor) { color = newColor; }
	Color GetColor() const { return color; }
	void SetFont(s8 newFont) { font = newFont; }

	std::string TextContent() { return textContent; }
	void TextContent(std::string newText);

	void Draw();
private:
	// Set the anchor position of the text object (the size is based on the text content string width and height)
	void SetAnchorPosition(Transform const& transform);
	// Set the anchor position of the text to be printed out (In Normalized coordinates)
	void SetTextAnchorPosition();
private:
	std::string textContent = "";
	std::string prevTextContent = "";

	float textScale = 1.0f;
	Transform transform;
	Color color{ 1,1,1 };
	s8 font;

	Transform parent{ {0,0}, {0,0} };
	Utils::AnchorMode anchorMode = Utils::AnchorMode::CENTER;
	// position of the textObject is affect by its text content and has a parent object like a panel 
	// need to update its position whenever text is update
	bool isSettingAnchorPos = false; 
	bool hasUpdatePos = false;

	AEVec2 offsetPos{ 0,0 };
};

#endif // !TEXTOBJECT_HPP
