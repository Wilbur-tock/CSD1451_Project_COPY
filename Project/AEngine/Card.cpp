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
	: button{ TextObject{ _name, 0.6f, Color{1,1,1}, {{0,0},{_size.x - (25*2.0f), 0}}}, Utils::AnchorMode::CENTER, _pos, _size, _color, _cardtexture, nullptr, _onClick}
	, pos{ _pos }, imageTexture{ _imageTexture }, title{ TextObject{"TITLE"}} {

	// Set button text object anchor position
	TextObject& textObj = button.GetTextObject();
	textObj.SetPosition(Utils::AnchorMode::CENTER_LEFT, button.GetTransform());
	textObj.SetOffsetPosition(25, -25.0f);

	// set title position to be at the top
	title.SetPosition(Utils::AnchorMode::TOP_CENTER, { _pos, _size });
	title.SetOffsetPosition(0, -10.0f);
}

void Card::Update() {
	button.Update();
}

void Card::Draw() {
	// Draw the background of the card and title
	button.Draw();
	title.Draw();
	
	// Draw the ability icon on top of the card
	if (imageTexture) {
		AEVec2 imagePos = { pos.x + (CARD_WIDTH - IMAGE_WIDTH) / 2, pos.y - 50.0f};
		GraphicsManager::DrawScreenOverlay(imageTexture, GraphicsManager::CreateTransformMatrix(imagePos.x, imagePos.y, IMAGE_WIDTH, IMAGE_HEIGHT, 0), Color(1,1,1));
	}
}

void Card::SetIconTexture(AEGfxTexture* texture) {
	imageTexture = texture;
}