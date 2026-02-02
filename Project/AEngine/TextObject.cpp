#include "TextObject.hpp"

TextObject::TextObject(std::string _textContent, float _scale, Color _color, AEVec2 _pos)
	: textContent{ _textContent }, textScale{ _scale }, color{ _color }, pos{ _pos } {}
