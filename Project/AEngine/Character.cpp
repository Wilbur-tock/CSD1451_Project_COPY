#include "Character.hpp"

#include "UIManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "PhysicsManager.hpp"
#include "Utils.hpp"
#include "CollisionManager.hpp"

namespace {
	// ---- Change these paths to your real filenames if different ----
	static const char* PLAYER_IDLE_PATH = "Assets/playerIdle.png";
	static const char* PLAYER_HALFHP_PATH = "Assets/playerHalfHp.png";
	static const char* PLAYER_SQUAT_PATH = "Assets/playerSquat.png";

	static const char* ENEMY_100_PATH = "Assets/enemy100.png";
	static const char* ENEMY_75_PATH = "Assets/enemy75.png";
	static const char* ENEMY_50_PATH = "Assets/enemy50.png";
	static const char* ENEMY_25_PATH = "Assets/enemy25.png";
	static const char* ENEMY_BOSS_PATH = "Assets/enemyBoss.png";

	static bool gCharTexLoaded = false;

	static AEGfxTexture* gPlayerIdleTex = nullptr;
	static AEGfxTexture* gPlayerHalfHpTex = nullptr;
	static AEGfxTexture* gPlayerSquatTex = nullptr;

	static AEGfxTexture* gEnemy100Tex = nullptr;
	static AEGfxTexture* gEnemy75Tex = nullptr;
	static AEGfxTexture* gEnemy50Tex = nullptr;
	static AEGfxTexture* gEnemy25Tex = nullptr;
	static AEGfxTexture* gEnemyBossTex = nullptr;

	static float PLAYER_SQUAT_FRAME_INTERVAL = 0.15f; // seconds per frame toggle

	static AEGfxTexture* FallbackTex() {
		// Prefer cached squareTexture if you have it
		if (AssetsManager::squareTexture) return AssetsManager::squareTexture;
		return AssetsManager::GetTexture("Assets/square.png");
	}

	static void LoadCharacterTexturesOnce() {
		if (gCharTexLoaded) return;
		gCharTexLoaded = true;

		AEGfxTexture* fallback = FallbackTex();

		gPlayerIdleTex = AssetsManager::GetTexture(PLAYER_IDLE_PATH);
		gPlayerHalfHpTex = AssetsManager::GetTexture(PLAYER_HALFHP_PATH);
		gPlayerSquatTex = AssetsManager::GetTexture(PLAYER_SQUAT_PATH);

		gEnemy100Tex = AssetsManager::GetTexture(ENEMY_100_PATH);
		gEnemy75Tex = AssetsManager::GetTexture(ENEMY_75_PATH);
		gEnemy50Tex = AssetsManager::GetTexture(ENEMY_50_PATH);
		gEnemy25Tex = AssetsManager::GetTexture(ENEMY_25_PATH);
		gEnemyBossTex = AssetsManager::GetTexture(ENEMY_BOSS_PATH);

		// Fallbacks (avoid nullptr crashes)
		if (!gPlayerIdleTex)   gPlayerIdleTex = fallback;
		if (!gPlayerHalfHpTex) gPlayerHalfHpTex = gPlayerIdleTex;
		if (!gPlayerSquatTex)  gPlayerSquatTex = gPlayerIdleTex;

		if (!gEnemy100Tex) gEnemy100Tex = fallback;
		if (!gEnemy75Tex)  gEnemy75Tex = gEnemy100Tex;
		if (!gEnemy50Tex)  gEnemy50Tex = gEnemy75Tex;
		if (!gEnemy25Tex)  gEnemy25Tex = gEnemy50Tex;
		if (!gEnemyBossTex) gEnemyBossTex = gEnemy100Tex;
	}
}

Character::Character(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId, CharacterType _type)
	: GameObject{ _pos, _size, _texture, _tileId, Color{1,1,1,1} }, type{ _type } {
	attribute.MaxHealth(100);
	LoadCharacterTexturesOnce();
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

void Character::UpdateSprite() {
	// Defensive: avoid divide-by-zero
	float hpRatio = 1.0f;
	if (attribute.MaxHealth() > 0) {
		hpRatio = static_cast<float>(attribute.Health()) / static_cast<float>(attribute.MaxHealth());
	}

	if (type == CharacterType::PLAYER) {
		// Base frame: idle OR half-hp
		AEGfxTexture* baseFrame = (hpRatio <= 0.5f) ? gPlayerHalfHpTex : gPlayerIdleTex;

		// Squat animation: toggle between base and squat frame
		if (squatAnimEnabled) {
			squatAnimTimer += PhysicsManager::g_dt;
			if (squatAnimTimer >= PLAYER_SQUAT_FRAME_INTERVAL) {
				squatAnimTimer -= PLAYER_SQUAT_FRAME_INTERVAL;
				squatAnimShowSquat = !squatAnimShowSquat;
			}
			texture = squatAnimShowSquat ? gPlayerSquatTex : baseFrame;
		}
		else {
			texture = baseFrame;
		}
	}
	else if (type == CharacterType::ENEMY_BOSS) {
		// Boss uses its own dedicated sprite
		texture = gEnemyBossTex;
	}
	else {
		// Regular enemy minion uses HP-based sprites 100%/75%/50%/25%
		if (hpRatio > 0.75f)      texture = gEnemy100Tex;
		else if (hpRatio > 0.50f) texture = gEnemy75Tex;
		else if (hpRatio > 0.25f) texture = gEnemy50Tex;
		else                      texture = gEnemy25Tex;
	}

	if (!texture) texture = FallbackTex();
}

void Character::Draw() {
	UpdateSprite(); // Update character sprite texture
	GraphicsManager::DrawGameObject(*this);
}

void Character::DrawUI() {
	// UI
	float x = transform.pos.x - transform.size.x * 0.5f, y = transform.pos.y + transform.size.y;
	UIManager::Bar healthBar{ { x, y}, { transform.size.x, 10 }, Color{1,0,0} };
	healthBar.Draw(static_cast<float>(attribute.Health()), static_cast<float>(attribute.MaxHealth()), false);

	if (attribute.Shield() > 0) {
		float width = 5;
		UIManager::Bar shieldBar{ {x,y + width + 2.5f} , { transform.size.x, width }, Color{1.0f,1.0f,0 } };
		shieldBar.Draw(static_cast<float>(attribute.Shield()), static_cast<float>(attribute.MaxShield()), false);
	}

	//if (CollisionManager::IsPointInGameObject(*this)) {
	//	float w = 250, h = 100;
	//	UIManager::Panel statsPanel{ {w,h}, {1,1,1}, Utils::AnchorMode::TOP_LEFT };
	//	statsPanel.AddOffset({0,-25});
	//	statsPanel.Draw();

	//	UIManager::Bar statsPanelHPBar{ statsPanel.Pos(), {w,15}, {1,0,0} };
	//	statsPanelHPBar.Draw(static_cast<float>(attribute.Health()), static_cast<float>(attribute.MaxHealth()));

	//	if (attribute.Shield() > 0) {
	//		UIManager::Bar shieldBar{ {statsPanel.Pos().x, statsPanel.Pos().y - 15} , {w, 10}, Color{1.0f,1.0f,0}};
	//		shieldBar.Draw(static_cast<float>(attribute.Shield()), static_cast<float>(attribute.MaxShield()));
	//	}
	//}

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
		+ "\nIsGrounded: " + static_cast<std::string>(physics.IsGrounded() ? "true" : "false")
		+ "\n\nStats:\nHP: " + std::to_string(attribute.Health()) + "/" + std::to_string(attribute.MaxHealth())
		+ "\nDamage Multiplier: " + std::to_string(attribute.DamageMultiplier())
		+ "\nShield: " + std::to_string(attribute.Shield()) + "/" + std::to_string(attribute.MaxShield());
}

void Character::DebugInfo() {
	if (!Utils::isDebuggerOn) return;

	// Draw Collision line
	GraphicsManager::DrawLine(transform.pos, collider.size);
}

void Character::SetStats(int difficultyLevel) {
	if (type == CharacterType::ENEMY_BOSS) {
		int multipler = difficultyLevel + 1; 
		int maxHP = 100 * multipler;
		attribute.MaxHealth(maxHP);
		attribute.Health(maxHP);

		int shield = 10 * multipler;
		attribute.Shield(shield);
		attribute.MaxShield(shield);

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