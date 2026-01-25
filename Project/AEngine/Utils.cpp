#include "Utils.hpp"

Color::Color() {
	SetColor(0, 0, 0, 0);
}
Color::Color(float _r, float _g, float _b, float _a) {
	SetColor(_r, _g, _b, _a);
}

// string text, aevec2 _pos, width, float, color, texture
Button::Button(std::string _text, AEVec2 _pos, float _width, float _height, Color _color, AEGfxTexture* _texture) 
	: text{_text}, pos{ _pos }, width{_width}, height{_height}, color{ _color }, texture{_texture} {
	// Add offset with the origin being top left corner of the screen
	//pos.x -= Config::SCREEN_WIDTH / 2.0f;
	//pos.y += (Config::SCREEN_HEIGHT / 2.0f);
}

void Color::SetColor(float _r, float _g, float _b, float _a) {
	r = _r < min ? min : _r > max ? max : _r;
	g = _g < min ? min : _g > max ? max : _g;
	b = _b < min ? min : _b > max ? max : _b;
	a = _a < min ? min : _a > max ? max : _a;
}

float Color::GetRed() {
	return r;
}

float Color::GetGreen() {
	return g;
}

float Color::GetBlue() {
	return b;
}

float Color::GetAlpha() {
	return a;
}

namespace Utils {
	void GetWorldMousePos(float& x, float& y) {
		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		GetWorldPos(x, y);
		x += mouseX;
		y += mouseY;
	}

	void GetWorldPos(float& x, float& y) {
		f32 camX, camY;
		AEGfxGetCamPosition(&camX, &camY);

		// Adding the offset from camera moving and add the screenWidth if the mapWidth is larger than the screenWidth
		x += camX + Config::SCREEN_WIDTH;
		y += camY; // If screen height is the same as screen height no offset is needed
	}
}