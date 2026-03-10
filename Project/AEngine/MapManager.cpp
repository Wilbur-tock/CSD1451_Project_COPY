#include "MapManager.hpp"

#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Color.hpp"

namespace MapManager {
	std::vector<std::vector<Tile>> *currentMap;

	static const char* FLOOR_TOP_PATH = "Assets/floorTop.png";  // top layer floor
	static const char* FLOOR_FILL_PATH = "Assets/floorBody.png";  // underground fill of floor

	static const char* TOWER_BLOCK_TOP_1_PATH = "Assets/towerBlockTop1.png"; // Tower Block Top (1)
	static const char* TOWER_BLOCK_1_PATH = "Assets/towerBlockBody1.png"; // Tower Block Body (1)
	static const char* TOWER_BLOCK_TOP_2_PATH = "Assets/towerBlockTop2.png"; // Tower Block Top (2)
	static const char* TOWER_BLOCK_2_PATH = "Assets/towerBlockBody2.png"; // Tower Block Body (2)

	static const char* CANNON_BASE_PATH = "Assets/cannonBase.png"; //Cannon (for testing now, not actual sprite)
	static const char* PLAYER_IDLE_PATH = "Assets/playerIdle.png";      //player
	static const char* ENEMY_100_PATH = "Assets/enemy100.png";        //enemy (full hp)
	//Cracks textures for blocks based on their health (light, medium, heavy)
	static const char* CRACKS_LIGHT_PATH = "Assets/lightCracks.png";
	static const char* CRACKS_MEDIUM_PATH = "Assets/mediumCracks.png";
	static const char* CRACKS_HEAVY_PATH = "Assets/heavyCracks.png";
	// Cached textures
	static AEGfxTexture* gTileTex[static_cast<int>(TileType::TILE_COUNT)] = { nullptr };
	static AEGfxTexture* gFloorTopTex = nullptr;
	static AEGfxTexture* gFloorFillTex = nullptr;
	static AEGfxTexture* gTower1TopTex = nullptr;
	static AEGfxTexture* gTower1BodyTex = nullptr;
	static AEGfxTexture* gTower2TopTex = nullptr;
	static AEGfxTexture* gTower2BodyTex = nullptr;
	static AEGfxTexture* gCannonBaseTex = nullptr;
	static AEGfxTexture* gFallbackTex = nullptr;
	static AEGfxTexture* gPlayerIdleTex = nullptr;
	static AEGfxTexture* gEnemy100Tex = nullptr;
	static AEGfxTexture* gCracksLightTex = nullptr;
	static AEGfxTexture* gCracksMediumTex = nullptr;
	static AEGfxTexture* gCracksHeavyTex = nullptr;

	void Load() {
		if (gTileTex[static_cast<int>(TileType::TILE_BLOCK_GROUND_0)]) return;	

		gFallbackTex = AssetsManager::GetTexture("Assets/square.png");
		gFloorTopTex = AssetsManager::GetTexture(FLOOR_TOP_PATH);
		gFloorFillTex = AssetsManager::GetTexture(FLOOR_FILL_PATH);
		gTower1TopTex = AssetsManager::GetTexture(TOWER_BLOCK_TOP_1_PATH);
		gTower1BodyTex = AssetsManager::GetTexture(TOWER_BLOCK_1_PATH);
		gTower2TopTex = AssetsManager::GetTexture(TOWER_BLOCK_TOP_2_PATH);
		gTower2BodyTex = AssetsManager::GetTexture(TOWER_BLOCK_2_PATH);
		gCannonBaseTex = AssetsManager::GetTexture(CANNON_BASE_PATH);
		gPlayerIdleTex = AssetsManager::GetTexture(PLAYER_IDLE_PATH);
		gEnemy100Tex = AssetsManager::GetTexture(ENEMY_100_PATH);
		gCracksLightTex = AssetsManager::GetTexture(CRACKS_LIGHT_PATH);
		gCracksMediumTex = AssetsManager::GetTexture(CRACKS_MEDIUM_PATH);
		gCracksHeavyTex = AssetsManager::GetTexture(CRACKS_HEAVY_PATH);

		// Safety Fallback
		if (!gFloorTopTex)  gFloorTopTex = gFallbackTex;
		if (!gFloorFillTex) gFloorFillTex = gFallbackTex;
		if (!gTower1TopTex)  gTower1TopTex = gFallbackTex;
		if (!gTower1BodyTex)  gTower1BodyTex = gFallbackTex;
		if (!gTower2TopTex)  gTower2TopTex = gFallbackTex;
		if (!gTower2BodyTex)  gTower2BodyTex = gFallbackTex;
		if (!gCannonBaseTex) gCannonBaseTex = gFallbackTex;
		if (!gPlayerIdleTex) gPlayerIdleTex = gFallbackTex;
		if (!gEnemy100Tex)   gEnemy100Tex = gFallbackTex;

		gTileTex[static_cast<int>(TileType::TILE_EMPTY)] = nullptr;
		// ground uses top/fill selection in RenderMap (this just marks “loaded”)
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_GROUND_0)] = gFloorTopTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_GROUND_1)] = gFloorFillTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_GROUND_2)] = gFloorFillTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_CANNON_BASE_TOP)] = gCannonBaseTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_CANNON_BASE)] = gTower1BodyTex; // CHANGE THIS

		// DO NOT overwrite these afterwards
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_1_TOP)] = gTower1TopTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_1)] = gTower1BodyTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_2_TOP)] = gTower2TopTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_2)] = gTower2BodyTex;

		// cannon base tile texture
		gTileTex[static_cast<int>(TileType::TILE_PLAYER_CANNON)] = gCannonBaseTex;
		gTileTex[static_cast<int>(TileType::TILE_ENEMY_CANNON)] = gCannonBaseTex;

		// other types can stay fallback for now
		gTileTex[static_cast<int>(TileType::TILE_PLAYER_CHARACTER)] = gPlayerIdleTex;
		gTileTex[static_cast<int>(TileType::TILE_ENEMY_MINION)] = gEnemy100Tex;
		gTileTex[static_cast<int>(TileType::TILE_ENEMY_BOSS)] = gFallbackTex;

		// Debug prints if missing
		if (gFloorTopTex == gFallbackTex) {
			std::cerr << "[MapManager] WARNING: missing floor top texture at "
				<< FLOOR_TOP_PATH << std::endl;
		}
		if (gFloorFillTex == gFallbackTex) {
			std::cerr << "[MapManager] WARNING: missing floor fill texture at "
				<< FLOOR_FILL_PATH << std::endl;
		}
		if (gCannonBaseTex == gFallbackTex) {
			std::cerr << "[MapManager] WARNING: missing cannon base texture at "
				<< CANNON_BASE_PATH << std::endl;
		}
		if (gTower1TopTex == gFallbackTex)
			std::cerr << "[MapManager] WARNING: missing towerBlockTop1 at " 
			<< TOWER_BLOCK_TOP_1_PATH << "\n";
		if (gTower1BodyTex == gFallbackTex)
			std::cerr << "[MapManager] WARNING: missing towerBlockBody1 at " 
			<< TOWER_BLOCK_1_PATH << "\n";
		if (gTower2TopTex == gFallbackTex)
			std::cerr << "[MapManager] WARNING: missing towerBlockTop2 at " 
			<< TOWER_BLOCK_TOP_2_PATH << "\n";
		if (gTower2BodyTex == gFallbackTex)
			std::cerr << "[MapManager] WARNING: missing towerBlockBody2 at " 
			<< TOWER_BLOCK_2_PATH << "\n";
		if (gCannonBaseTex == gFallbackTex)
			std::cerr << "[MapManager] WARNING: missing cannonBase at " 
			<< CANNON_BASE_PATH << "\n";

		// Debug prints if missing cracks
		if (!gCracksLightTex)
			std::cerr << "[MapManager] WARNING: missing lightCracks at " << CRACKS_LIGHT_PATH << "\n";
		if (!gCracksMediumTex)
			std::cerr << "[MapManager] WARNING: missing mediumCracks at " << CRACKS_MEDIUM_PATH << "\n";
		if (!gCracksHeavyTex)
			std::cerr << "[MapManager] WARNING: missing heavyCracks at " << CRACKS_HEAVY_PATH << "\n";
	}

	void Unload() {
		currentMap = nullptr;

		gFallbackTex = nullptr;
		gFloorTopTex = nullptr;
		gFloorFillTex = nullptr;
		gTower1TopTex = nullptr;
		gTower1BodyTex = nullptr;
		gTower2TopTex = nullptr;
		gTower2BodyTex = nullptr;
		gCannonBaseTex = nullptr;
		gPlayerIdleTex = nullptr;
		gEnemy100Tex = nullptr;
		gCracksLightTex = nullptr;
		gCracksMediumTex = nullptr;
		gCracksHeavyTex = nullptr;

		for (int i = 0; i < static_cast<int>(TileType::TILE_COUNT); ++i) gTileTex[i] = nullptr;
	}

	void ReadMapData(std::string const& fileName, std::vector<std::vector<Tile>> &map) {
		std::ifstream ifs{fileName};
		if (!ifs) {
			std::cerr << "Error: Could not open map data file at " << fileName << std::endl;
			return;
		}

		// Calculate how many tiles fit on screen
		int width = Config::GRID_WIDTH;
		int height = Config::GRID_HEIGHT;
		float tileSize = static_cast<float>(Config::TILE_SIZE);

		// Offset to start from the top left corner
		float startX = -((width * tileSize) / 2.0f) + (tileSize / 2.0f);
		float startY = ((height * tileSize) / 2.0f) - (tileSize / 2.0f);

		AEGfxTexture* tileTexture = AssetsManager::GetTexture("Assets/square.png");

		// Initialize each Tile class data in the 2D vector
		for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
			for (int x = 0; x < Config::GRID_WIDTH; ++x) {
				// Set type
				int type = 0;
				ifs >> type;

				if (type < 0 || type >= static_cast<int>(TileType::TILE_COUNT)) {
					std::cerr << "[MapManager] WARNING: invalid tile id " << type
						<< " at (" << x << "," << y << ") in " << fileName
						<< " -> forcing TILE_EMPTY\n";
					type = static_cast<int>(TileType::TILE_EMPTY);
				}
				else {
					tileTexture = gTileTex[type];
				}

				// Initialize each tile object here

				Tile& tile = map[y][x];
				tile.transform.pos = { startX + (x * tileSize), startY - (y * tileSize) };
				tile.transform.size = { Config::TILE_SIZE,Config::TILE_SIZE };
				tile.texture = tileTexture;
				tile.Type(static_cast<TileType>(type));
				tile.tileId = { x,y };
				tile.attribute = Attributes{};

				if (tile.IsBlocks()) {
					tile.attribute.DamageMultiplier(5);
				}
			}
		}

		// Assign currentMap to point to the map
		currentMap = &map;

		ifs.close();
	}

	void DrawMap(std::vector<std::vector<Tile>> const& map, bool drawCharacters) {
		AEGfxSetBackgroundColor(0.92f, 0.92f, 0.95f);

		int mapHeight = static_cast<int>(map.size());
		int mapWidth = static_cast<int>(map[0].size());

		if (mapHeight < 0 || mapHeight > Config::GRID_HEIGHT || mapWidth < 0 || mapWidth > Config::GRID_WIDTH) {
			std::cout << "[MapManager] Map out of bounds width: " << mapWidth << " height: " << mapHeight << '\n';
			return;
		}

		// Small helper to clamp without relying on C++17 std::clamp (safe either way)
		auto Clamp01 = [](float v) {
			if (v < 0.0f) return 0.0f;
			if (v > 1.0f) return 1.0f;
			return v;
			};

		for (std::vector<Tile> const& row : map) {
			for (Tile const& tile : row) {
				if (tile.Type() == TileType::TILE_EMPTY) continue;
				if (!drawCharacters && tile.IsCharacter()) continue;

				AEGfxTexture* baseTex = gTileTex[static_cast<int>(tile.Type())];
				if (!baseTex) continue;

				AEMtx33 tileMat = GraphicsManager::CreateTransformMatrix(
					tile.transform.pos.x, tile.transform.pos.y, 0.0f,
					tile.transform.size.x / 2.0f, tile.transform.size.y / 2.0f
				);

				// 1) Draw base tile
				GraphicsManager::DrawGameObject(baseTex, tileMat, Color{ 1,1,1,1 });

				// 2) Draw cracks overlay (only for ground + blocks)
				if (tile.IsBlocks()) {
					int hp = tile.attribute.Health();
					int maxHp = tile.attribute.MaxHealth();

					// Only show cracks once it has taken damage
					if (maxHp > 0 && hp < maxHp) {
						float damageFrac = 1.0f - (static_cast<float>(hp) / static_cast<float>(maxHp));
						damageFrac = Clamp01(damageFrac);

						// pick crack stage
						AEGfxTexture* cracksTex = nullptr;
						if (damageFrac < 0.33f) cracksTex = gCracksLightTex;
						else if (damageFrac < 0.66f) cracksTex = gCracksMediumTex;
						else cracksTex = gCracksHeavyTex;

						// If one is missing, fallback to any available crack texture
						if (!cracksTex) cracksTex = gCracksHeavyTex ? gCracksHeavyTex
							: (gCracksMediumTex ? gCracksMediumTex : gCracksLightTex);

						// draw overlay if we have it
						if (cracksTex) {
							float alpha = 0.20f + damageFrac * 0.65f; // 0.20 -> 0.85
							if (alpha > 0.85f) alpha = 0.85f;

							GraphicsManager::DrawGameObject(cracksTex, tileMat, Color{ 1,1,1,alpha });
						}
					}
				}
			}
		}
	}

	Tile* FindTile(TileIndex const& tileId) {
		for (std::vector<Tile>& list : *currentMap) {
			for (Tile& tile : list) {
				if (tile.tileId == tileId) {
					return &tile;
				}
			}
		}

		std::cerr << "[MapManager] Can't find Tile: " << tileId << std::endl;
		return nullptr;
	}
}