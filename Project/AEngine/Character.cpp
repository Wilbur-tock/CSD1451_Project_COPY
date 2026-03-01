#include "Character.hpp"

#include "UIManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "PhysicsManager.hpp"
#include "Utils.hpp"

namespace {
	// ---- Change these paths to your real filenames if different ----
	static const char* PLAYER_IDLE_PATH = "Assets/playerIdle.png";
	static const char* PLAYER_HALFHP_PATH = "Assets/playerHalfHp.png";
	static const char* PLAYER_SQUAT_PATH = "Assets/playerSquat.png";

	static const char* ENEMY_100_PATH = "Assets/enemy100.png";
	static const char* ENEMY_75_PATH = "Assets/enemy75.png";
	static const char* ENEMY_50_PATH = "Assets/enemy50.png";
	static const char* ENEMY_25_PATH = "Assets/enemy25.png";

	static bool gCharTexLoaded = false;

	static AEGfxTexture* gPlayerIdleTex = nullptr;
	static AEGfxTexture* gPlayerHalfHpTex = nullptr;
	static AEGfxTexture* gPlayerSquatTex = nullptr;

	static AEGfxTexture* gEnemy100Tex = nullptr;
	static AEGfxTexture* gEnemy75Tex = nullptr;
	static AEGfxTexture* gEnemy50Tex = nullptr;
	static AEGfxTexture* gEnemy25Tex = nullptr;

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

		// Fallbacks (avoid nullptr crashes)
		if (!gPlayerIdleTex)   gPlayerIdleTex = fallback;
		if (!gPlayerHalfHpTex) gPlayerHalfHpTex = gPlayerIdleTex;
		if (!gPlayerSquatTex)  gPlayerSquatTex = gPlayerIdleTex;

		if (!gEnemy100Tex) gEnemy100Tex = fallback;
		if (!gEnemy75Tex)  gEnemy75Tex = gEnemy100Tex;
		if (!gEnemy50Tex)  gEnemy50Tex = gEnemy75Tex;
		if (!gEnemy25Tex)  gEnemy25Tex = gEnemy50Tex;
	}
}

Character::Character(AEVec2 _pos, AEVec2 _size, AEGfxTexture* _texture, TileIndex _tileId, CharacterType _type)
	: GameObject{ _pos, _size, _texture, _tileId, Color{1,1,1,1} }, type{ _type } {
	attribute.MaxHealth(100);

	// With real sprites, tinting usually makes them look wrong. Keep white.
	textureColor = { 1,1,1,1 };

	LoadCharacterTexturesOnce();
	UpdateSprite();
}

CharacterType Character::Type() const { return type; }

void Character::Update() {
	physics.Update(*this);
	UpdateSprite();
	Render();
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
	if (!attribute.IsActive()) return;

	LoadCharacterTexturesOnce();

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
	else {
		// Enemy uses 4 sprites by HP range
		// >75% -> 100 sprite
		// >50% -> 75 sprite
		// >25% -> 50 sprite
		// else -> 25 sprite
		if (hpRatio > 0.75f)      texture = gEnemy100Tex;
		else if (hpRatio > 0.50f) texture = gEnemy75Tex;
		else if (hpRatio > 0.25f) texture = gEnemy50Tex;
		else                      texture = gEnemy25Tex;
	}

	if (!texture) texture = FallbackTex();
}

void Character::Render() {
	if (!attribute.IsActive()) return;

	GraphicsManager::DrawGameObject(*this);
	// UI
	float x = transform.pos.x, y = transform.pos.y + transform.size.y;
	UIManager::Bar bar{ { x, y}, { transform.size.x, 10 }, Color{1,0,0} };
	bar.Draw(static_cast<float>(attribute.Health()), static_cast<float>(attribute.MaxHealth()));
}

void Character::DebugInfo(std::string& str) {
	std::stringstream ss;
	ss << &*this; // Display address of the character object
	str = "\n\nCharacter Info: " + ss.str()
		+ "\nType: " + TypeName()
		+ "\nCharacter Id: " + std::to_string(id)
		+ "\nTile Id x: " + std::to_string(tileId.x) + " y : " + std::to_string(tileId.y)
		+ "\nPos x: " + Utils::FormatFloat(transform.pos.x, 2) + " y: " + Utils::FormatFloat(transform.pos.y, 2)
		+ "\nSize x: " + Utils::FormatFloat(transform.size.x, 2) + " y: " + Utils::FormatFloat(transform.size.x, 2)
		+ "\nIsGrounded: " + static_cast<std::string>(physics.IsGrounded() ? "true" : "false")
		+ "\n\nStats:\nHP: " + std::to_string(attribute.Health()) + " Max HP: " + std::to_string(attribute.MaxHealth())
		+ "\nDamage Multiplier: " + std::to_string(attribute.DamageMultiplier());
}