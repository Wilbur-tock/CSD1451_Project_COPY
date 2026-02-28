#ifndef TEXTOBJECT_HPP
#define TEXTOBJECT_HPP

#include "pch.hpp"

#include "Color.hpp"
#include "Utils.hpp"

class TextObject {
public:
	std::string textContent;
	float textScale = 1.0f;
	AEVec2 pos{ 0,0 };
	Color color;

	TextObject() = default;
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
	TextObject(std::string _text, float _scale = 1.0f, Color _color = Color{ 1,1,1 }, AEVec2 _pos = { 0,0 }, Utils::RectMode _rectMode = Utils::RectMode::RECT_TOP_LEFT);

	/*!***********************************************************************
	\brief
	Set text object position according to the parent panel
	\param[in] panelPos
	- position of the parent panel
	\param[in] panelSize
	- size of the panel
	*************************************************************************/
	void SetPos(AEVec2 panelPos, AEVec2 panelSize);
	void Draw();
private:
	Utils::RectMode rectMode = Utils::RectMode::RECT_CENTER;
};

#endif // !TEXTOBJECT_HPP
