#include "TextObject.hpp"

TextObject::TextObject(std::string _textContent, AEVec2 _pos, Color _color, float _scale) 
	: textContent{ _textContent }, pos{ _pos }, color{ _color }, scale{ _scale } {}
