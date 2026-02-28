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

	Character(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId);

	CharacterType Type() const;

	void Update();
	void Render();
	void DebugInfo(std::string &str);

	inline std::string TypeName() {
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
private:
	CharacterType type;
};

#endif // !CHARACTER_HPP
