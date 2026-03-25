#include "Character.hpp"

#include "UIManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "PhysicsManager.hpp"
#include "Utils.hpp"
#include "CollisionManager.hpp"

float barGaps = 2.5f;
static float PLAYER_SQUAT_FRAME_INTERVAL = 0.25f; // seconds per frame toggle

Character::Character(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId, CharacterType _type)
	: GameObject{ _pos, _size, _texture, _tileId, Color{1,1,1,1} }, type{ _type } {
	// Reserve vector capacity
	abilityModifiers.reserve(10);

	attribute.MaxHealth(100);

	// Initialize UI Objects
	Color hpBarColor = EnemyHPColor();
	if (_type == CharacterType::PLAYER) {
		hpBarColor = PlayerHPColor();
	}
	float wsBarX = transform.pos.x - transform.size.x * 0.5f, wsBarY = transform.pos.y + transform.size.y;
	float barHeight = 10;
	float wsShieldBarY = wsBarY + barHeight + barGaps;

	wsHPBar = UIManager::Bar{ { wsBarX, wsBarY}, { transform.size.x, barHeight }, hpBarColor }; // Position just on top of the character
	wsShieldBar = UIManager::Bar{ {wsBarX,wsShieldBarY} , { transform.size.x, barHeight * 0.5f }, ShieldColor()};

	statsPanel = UIManager::Panel{ {300,150}, PanelColor(), Utils::AnchorMode::BOTTOM_RIGHT };

	float gap = 2.5f;
	float borderGap = 5.0f;

	// Create the character icon
	AEVec2 iconSize{ 32,32 };
	icon = UIManager::Panel{ iconSize, {1,1,1}, Utils::AnchorMode::TOP_LEFT, texture };
	icon.SetChildPanelPos(statsPanel.GetTransform().pos, { borderGap,-borderGap });

	float colorTint = 0.9f;
	iconBG = UIManager::Panel{ iconSize, {colorTint,colorTint,colorTint}, Utils::AnchorMode::TOP_LEFT, };
	iconBG.SetChildPanelPos(statsPanel.GetTransform().pos, { borderGap,-borderGap });

	float barWidth = 15.0f;
	AEVec2 barPos{ statsPanel.GetTransform().pos.x + iconSize.x + borderGap, statsPanel.GetTransform().pos.y - borderGap };
	AEVec2 barSize{ statsPanel.GetTransform().size.x - iconSize.x - (borderGap * 2),  15.0f };

	soHPBar = UIManager::Bar{ {barPos}, {barSize}, hpBarColor };
	soShieldBar = UIManager::Bar{ {barPos.x, barPos.y - (barWidth + gap)}, {barSize}, ShieldColor()};

	statusText = TextObject{ "", 0.5f, Color{0.75f,0,0} };
	statusText.SetPosition(Utils::AnchorMode::CENTER_LEFT, statsPanel.GetTransform());

	//SetSquatAnimation(true);
}

void Character::Update() {
	if (!attribute.IsActive()) return;

	physics.Update(*this);

	Draw();
	DrawUI();
}

void Character::SetSquatAnimation(bool enabled) {
	// Only player uses squat animation, but safe to call anyway
	squatAnimEnabled = enabled;

	if (!enabled) {
		squatAnimTimer = 0.0f;
		squatAnimShowSquat = false;
	}
}

void Character::TextureUpdate() {
	// Defensive: avoid divide-by-zero
	float hpRatio = 1.0f;
	if (attribute.MaxHealth() > 0) {
		hpRatio = static_cast<float>(attribute.Health()) / static_cast<float>(attribute.MaxHealth());
	}

	std::vector<AEGfxTexture*> const& textures = AssetsManager::GetCharacterTexture(static_cast<int>(type));

	if (type == CharacterType::PLAYER) {
		// Base frame: idle OR half-hp
		AEGfxTexture* baseFrame = (hpRatio <= 0.5f) ? textures[1] : textures[0];

		// Squat animation: toggle between base and squat frame
		if (squatAnimEnabled) {
			squatAnimTimer += PhysicsManager::g_dt;
			if (squatAnimTimer >= PLAYER_SQUAT_FRAME_INTERVAL) {
				squatAnimTimer -= PLAYER_SQUAT_FRAME_INTERVAL;
				squatAnimShowSquat = !squatAnimShowSquat;
			}
			texture = squatAnimShowSquat ? textures[2] : baseFrame;
		}
		else {
			texture = baseFrame;
		}
	}
	else if (type == CharacterType::ENEMY_BOSS) {
		// Boss uses its own dedicated sprite
		texture = textures[0];
	}
	else {
		// Regular enemy minion uses HP-based sprites 100%/75%/50%/25%
		if (hpRatio > 0.75f)      texture = textures[0];
		else if (hpRatio > 0.50f) texture = textures[1];
		else if (hpRatio > 0.25f) texture = textures[2];
		else                      texture = textures[3];
	}
}

void Character::Draw() {
	TextureUpdate(); // Update character sprite texture

	GraphicsManager::DrawCharacter(*this);

	// Draw shield
	if (attribute.IsShieldActive()) {
		float shieldSize = Config::TILE_SIZE;
		GraphicsManager::DrawGameObject(
			AssetsManager::circleTexture,
			GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, shieldSize, shieldSize, 0),
			Color{ 0.9f,0.9f,0, 0.3f }
		);
	}
}

void Character::DrawUI() {
	float hp = static_cast<float>(attribute.Health());
	float maxHp = static_cast<float>(attribute.MaxHealth());
	float shield = static_cast<float>(attribute.Shield());
	float maxShield = static_cast<float>(attribute.MaxShield());

	bool drawShieldStats = attribute.IsShieldActive();

	// Draw and update position according to the character pos
	wsHPBar.UpdatePos({ transform.pos.x - transform.size.x * 0.5f, transform.pos.y + transform.size.y });
	wsHPBar.Draw(hp, maxHp, false);
	if (drawShieldStats) {
		wsShieldBar.UpdatePos({ wsHPBar.GetTransform().pos.x, wsHPBar.GetTransform().pos.y + wsShieldBar.GetTransform().size.y + barGaps });
		wsShieldBar.Draw(shield, maxShield, false);
	}

	// Drawing stats if player mouse is hovering over the character
	if (CollisionManager::IsPointInGameObject(*this)) {
		statsPanel.Draw();

		iconBG.Draw();
		icon.Draw();

		soHPBar.Draw(hp, maxHp);

		if (drawShieldStats) {
			soShieldBar.Draw(shield, maxShield);
		}

		if (attribute.IsTickDamageActive()) {
			statusText.TextContent("Poison: " + std::to_string(attribute.RemainingTicks()) + " rounds remaining");
			statusText.Draw();
		}
	}

	DebugInfo();
}

void Character::GetCharacterInfo(std::string& str) {
	std::stringstream ss;
	ss << &*this; // Display address of the character object
	str = "\n\nCharacter Info: " + ss.str()
		+ "\nType: " + TypeName()
		+ "\nCharacter Id: " + std::to_string(id)
		+ "\nTile Id x: " + std::to_string(tileId.x) + " y : " + std::to_string(tileId.y)
		+ "\nPos x: " + Utils::FormatFloat(transform.pos.x, 2) + " y: " + Utils::FormatFloat(transform.pos.y, 2)
		+ "\nSize x: " + Utils::FormatFloat(transform.size.x, 2) + " y: " + Utils::FormatFloat(transform.size.x, 2)
		+ "\nIsGrounded: " + std::string(physics.IsGrounded() ? "true" : "false")
		+ "\nCanBeDamaged: " + std::string(attribute.CanBeDamaged() ? "true" : "false")
		+ "\nIsPoison: " + std::string(attribute.IsTickDamageActive() ? "true" : "false") + attribute.TickDamageInfo()
		+ "\n\nStats:\nHP: " + std::to_string(attribute.Health()) + "/" + std::to_string(attribute.MaxHealth())
		+ "\nDamage Multiplier: " + std::to_string(attribute.DamageMultiplier())
		+ "\nShield: " + std::to_string(attribute.Shield()) + "/" + std::to_string(attribute.MaxShield());
}

void Character::DebugInfo() {
	if (!Utils::isDebuggerOn) return;

	DrawCollisionLine();
}

void Character::SetStats(int difficultyLevel) {
	if (type == CharacterType::ENEMY_BOSS) {
		int multipler = difficultyLevel + 1; 
		int maxHP = 100 * multipler;
		attribute.MaxHealth(maxHP);
		attribute.Health(maxHP);

		int shield = 5 * multipler;
		attribute.SetShield(shield);

		float dmgReduc = 0.0f;
		switch (multipler) {
		case 2:
			dmgReduc = 0.25f;
			break;
		case 3:
			dmgReduc = 0.5f;
			break;
		}

		attribute.DamageReduction(dmgReduc);
	}
}