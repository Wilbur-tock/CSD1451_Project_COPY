#ifndef TEXTOBJECT_HPP
#define TEXTOBJECT_HPP

#include "pch.hpp"

#include "Color.hpp"

class TextObject {
public:
	std::string textContent;
	float textScale = 1.0f;
	AEVec2 pos{ 0,0 };
	Color color;

	TextObject() = default;
	TextObject(std::string _text, float _scale = 1.0f, AEVec2 _pos = {0,0}, Color _color = Color{ 1,1,1 });
};

#endif // !TEXTOBJECT_HPP
