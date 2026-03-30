/*
Authors: Justin
Description: Display UI data about abilities to be selected

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef CARD_HPP
#define CARD_HPP

#include "pch.hpp"

#include "Color.hpp"
#include "Utils.hpp"
#include "TextObject.hpp"
#include "Button.hpp"

/*
Card class is used to display the different type of abilities the player can select from in the ability selection panel,
it is similar to the button class but with a different design and it is not interactable, 
the top of the card will display the image of the ability,
and the bottom of the card will display the name of the ability and the description of the ability
*/

#define CARD_WIDTH 350.0f
#define CARD_HEIGHT 550.0f
#define IMAGE_WIDTH 200.0f
#define IMAGE_HEIGHT 200.0f

class Card {
public:
	Card() = default;
	Card(std::string _name,
		AEVec2 _pos, 
		Color _color, 
		AEGfxTexture* _cardtexture, 
		AEGfxTexture* _imageTexture,
		Utils::FuncPtr _onClick = nullptr, 
		AEVec2 _size = { CARD_WIDTH, CARD_HEIGHT }
	);


	Button& GetButton() { return button; }
	TextObject& GetTitle() { return title; }

	void Update();
	void Draw();
	void SetIconTexture(AEGfxTexture* texture);
	void SetBGColor(Color newColor) { button.SetColor(newColor); }

private:
	std::string name;
	AEVec2 pos{ 0,0 };
	TextObject title;
	Button button; 
	AEGfxTexture* imageTexture = nullptr;
	Color imageTextureColor{ 1,1,1 };
};

#endif // !CARD_HPP