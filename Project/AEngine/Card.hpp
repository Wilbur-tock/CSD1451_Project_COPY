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
#define IMAGE_WIDTH 300.0f
#define IMAGE_HEIGHT 220.0f

class Card {
public:
	Card(std::string _name,
		AEVec2 _pos, 
		Color _color, 
		AEGfxTexture* _cardtexture, 
		AEGfxTexture* _imageTexture,
		Utils::FuncPtr _onClick = nullptr, 
		AEVec2 _size = { CARD_WIDTH, CARD_HEIGHT }
	);

	void Update();
	void Draw();

private:
	std::string name;
	Button button; 
	AEGfxTexture* imageTexture = nullptr;
};

//class Card {
//public:
//
//	Card() = default;
//	Card(AEVec2 _pos, AEVec2 _size, Color _color, int abilityId, Utils::FuncPtr _onClick = nullptr);
//
//	Card(TextObject _textObj, AEVec2 _pos, AEVec2 _size, Color _color,
//		int abilityId, Utils::FuncPtr _onClick = nullptr);
//
//	// Check for player hover and whatever or not it is clicked
//	void Update();
//	void Draw();
//
//private:
//	TextObject textObj;
//	AEVec2 pos{ 0,0 }, size{ CARD_WIDTH,CARD_HEIGHT };
//	Color color{ 1,1,1 };
//	AEGfxTexture* cardTexture = nullptr;
//	AEGfxTexture* imageTexture = nullptr;
//	Utils::FuncPtr onClick = nullptr;
//	bool isHovered = false;
//	Button button; // button object to be used for the card, the button will be used to check for hover and click events
//};
//



#endif // !CARD_HPP