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
	pos = _pos;
	imageTexture = _imageTexture;
}

void Card::Update() {
	button.Update();
}

void Card::Draw() {
	button.Draw();
	
	// Draw the image of the ability on top of the card
	if (imageTexture) {
		AEVec2 imagePos = { pos.x + (CARD_WIDTH - IMAGE_WIDTH) / 2, pos.y - 50.0f};
		GraphicsManager::DrawScreenOverlay(imageTexture, GraphicsManager::CreateTransformMatrix(imagePos.x, imagePos.y, 0, IMAGE_WIDTH, IMAGE_HEIGHT), Color(1,1,1));
	}
}