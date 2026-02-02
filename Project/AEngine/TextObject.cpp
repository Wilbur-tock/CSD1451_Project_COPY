#include "TextObject.hpp"

TextObject::TextObject(std::string _textContent, float _scale, AEVec2 _pos, Color _color)
	: textContent{ _textContent }, textScale{ _scale }, pos{ _pos }, color{ _color } {}
