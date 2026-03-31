/*
Authors: Dayrius
Description: Stores data and functions about each character unit in the game

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/


#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "pch.hpp"

#include "Attributes.hpp"
#include "GameObject.hpp" // Using GameObject and TileIndex.hpp
#include "Physics.hpp"
#include "UIManager.hpp"
#include "Ability.hpp"

enum class CharacterType {
	PLAYER,
	ENEMY_MINION,
	ENEMY_BOSS,
};

class Character : public GameObject {
public:
	Attributes attribute;
	Physics physics;

	Character(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId, CharacterType _type);

	CharacterType Type() const { return type; }
	bool IsPlayer() const { return type == CharacterType::PLAYER; }

	void Update();
	void Draw();
	void DrawUI();
	void DrawScreenOverlayUI();

	void GetCharacterInfo(std::string &str);
	void SetSquatAnimation(bool enabled); // Player only: toggles idle <-> squat animation

	std::string TypeName() {
		switch (type) {
		case CharacterType::PLAYER:
			return "PLAYER";
		case CharacterType::ENEMY_MINION:
			return "ENEMY_MINION";
		case CharacterType::ENEMY_BOSS:
			return "ENEMY_BOSS";
		}

		return "NULL";
	}

	void SetStats(int difficultyLevel);

private:
	// Print out information about the character and draw collision lines
	void DebugInfo();
	void TextureUpdate(); // Select correct sprite based on HP / animation state

private:
	CharacterType type;
	std::vector<Ability> abilityModifiers; // list of ability modifiers that is current apply on this character object

	// Player squat animation state
	bool squatAnimEnabled = false;
	float squatAnimTimer = 0.0f;
	bool squatAnimShowSquat = false;

	// UI Objects

	UIManager::Panel statsPanel, iconBG, icon;
	TextObject statusText;
	// ws = world space
	UIManager::Bar wsHPBar, wsShieldBar;
	// so = screen overlay
	UIManager::Bar soHPBar, soShieldBar;
};

#endif // !CHARACTER_HPP
