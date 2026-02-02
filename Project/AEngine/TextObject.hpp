#ifndef TEXTOBJECT_HPP
#define TEXTOBJECT_HPP

#include "pch.hpp"

#include "Color.hpp"

class TextObject {
public:
	std::string textContent;
	AEVec2 pos{ 0,0 };
	Color color;
	float textScale = 1.0f;

	TextObject() = default;
	TextObject(std::string _text, AEVec2 _pos = {0,0}, Color _color = Color{ 1,1,1 }, float _scale = 1.0f);
};

#endif // !TEXTOBJECT_HPP
