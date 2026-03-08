#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "pch.hpp"

#include "Attributes.hpp"
#include "GameObject.hpp" // Using GameObject and TileIndex.hpp
#include "Physics.hpp"

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

	void Update();
	void Draw();
	void DebugInfo(std::string &str);
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
	CharacterType type;

	// Player squat animation state
	bool squatAnimEnabled = false;
	float squatAnimTimer = 0.0f;
	bool squatAnimShowSquat = false;

	void UpdateSprite(); // Select correct sprite based on HP / animation state
};

#endif // !CHARACTER_HPP
