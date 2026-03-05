#include "Card.hpp"
#include "AssetsManager.hpp"
#include "UIManager.hpp"
#include "GraphicsManager.hpp"
#include "Button.hpp"

Card::Card(std::string _name,
	AEVec2 _pos,
	Color _color,
	AEGfxTexture* _cardtexture,
	AEGfxTexture* _imageTexture,
	Utils::FuncPtr _onClick,
	AEVec2 _size) 
	: button{ TextObject{ _name }, Utils::RectMode::RECT_BOTTOM_CENTER, _pos, _size, _color, _cardtexture, _onClick} {
}

void Card::Update() {
	button.Update();
}

void Card::Draw() {
	button.Draw();
	
	// Draw the image of the ability on top of the card
	/*if (imageTexture) {
		AEVec2 imagePos = { button.pos.x + (button.size.x - IMAGE_WIDTH) / 2.0f, button.pos.y + (button.size.y - IMAGE_HEIGHT) / 2.0f - 50.0f };
		GraphicsManager::DrawWorldUI(imageTexture, GraphicsManager::CreateTransformMatrix(imagePos.x, imagePos.y, 0, IMAGE_WIDTH, IMAGE_HEIGHT), Color(1,1,1));
	}*/
}