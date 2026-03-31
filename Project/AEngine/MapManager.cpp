/*
Authors: Dayrius, Bryan
Description: Load/unload map textures, read the map data file and draw the map layout

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "MapManager.hpp"

#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Color.hpp"
#include "GameTrackingManager.hpp"

namespace MapManager {
	std::vector<std::vector<Tile>> *currentMap; 

	static const char* FLOOR_TOP_PATH = "Assets/Tiles/floorTop.png";  // top layer floor
	static const char* FLOOR_FILL_PATH = "Assets/Tiles/floorBody.png";  // underground fill of floor

	static const char* TOWER_BLOCK_TOP_1_PATH = "Assets/Tiles/towerBlockTop1.png"; // Tower Block Top (1)
	static const char* TOWER_BLOCK_1_PATH = "Assets/Tiles/towerBlockBody1.png"; // Tower Block Body (1)
	static const char* TOWER_BLOCK_TOP_2_PATH = "Assets/Tiles/towerBlockTop2.png"; // Tower Block Top (2)
	static const char* TOWER_BLOCK_2_PATH = "Assets/Tiles/towerBlockBody2.png"; // Tower Block Body (2)

	static const char* CANNON_BASE_TOP_PATH = "Assets/Tiles/cannonBaseTop.png";
	static const char* CANNON_BASE_PATH = "Assets/Tiles/cannonBase.png";
	
	//Cracks textures for blocks based on their health (light, medium, heavy)
	static const char* CRACKS_LIGHT_PATH = "Assets/Tiles/cracksLight.png";
	static const char* CRACKS_MEDIUM_PATH = "Assets/Tiles/cracksMedium.png";
	static const char* CRACKS_HEAVY_PATH = "Assets/Tiles/cracksHeavy.png";
	// Cached textures
	static AEGfxTexture* gTileTex[static_cast<int>(TileType::TILE_COUNT)] = { nullptr };
	static AEGfxTexture* gFloorTopTex = nullptr;
	static AEGfxTexture* gFloorFillTex = nullptr;
	static AEGfxTexture* gTower1TopTex = nullptr;
	static AEGfxTexture* gTower1BodyTex = nullptr;
	static AEGfxTexture* gTower2TopTex = nullptr;
	static AEGfxTexture* gTower2BodyTex = nullptr;
	static AEGfxTexture* gCannonBaseTopTex = nullptr;
	static AEGfxTexture* gCannonBaseTex = nullptr;
	static AEGfxTexture* gFallbackTex = nullptr;
	static AEGfxTexture* gCracksLightTex = nullptr;
	static AEGfxTexture* gCracksMediumTex = nullptr;
	static AEGfxTexture* gCracksHeavyTex = nullptr;

	static float GetBlockDamageFraction(Tile const& tile) {
		int hp = tile.attribute.Health();
		int maxHp = tile.attribute.MaxHealth();

		if (maxHp <= 0 || hp >= maxHp) {
			return 0.0f;
		}

		float damageFrac = 1.0f - (static_cast<float>(hp) / static_cast<float>(maxHp));
		return std::clamp(damageFrac, 0.0f, 1.0f);
	}

	static Color BuildDamagedBlockColor(Tile const& tile, float damageFrac) {
		// Stronger darkening per damage stage so the block itself reads as damaged
		float tintMultiplier = 1.0f;

		if (damageFrac < 0.33f) {
			tintMultiplier = 0.90f; // light damage
		}
		else if (damageFrac < 0.66f) {
			tintMultiplier = 0.82f; // medium damage
		}
		else {
			tintMultiplier = 0.72f; // heavy damage
		}

		return Color{
			tile.textureColor.Red() * tintMultiplier,
			tile.textureColor.Green() * tintMultiplier,
			tile.textureColor.Blue() * tintMultiplier,
			tile.textureColor.Alpha()
		};
	}

	//Editing of cracked texture
	static void GetCrackVisualSettings(
		float damageFrac,
		AEGfxTexture*& cracksTex,
		float& primaryAlpha,
		float& secondaryAlpha)
	{
		cracksTex = nullptr;
		primaryAlpha = 0.0f;
		secondaryAlpha = 0.0f;

		if (damageFrac < 0.33f) {
			cracksTex = gCracksLightTex;
			primaryAlpha = 0.68f;
			secondaryAlpha = 0.16f;
		}
		else if (damageFrac < 0.66f) {
			cracksTex = gCracksMediumTex;
			primaryAlpha = 0.82f;
			secondaryAlpha = 0.24f;
		}
		else {
			cracksTex = gCracksHeavyTex;
			primaryAlpha = 0.96f;
			secondaryAlpha = 0.34f;
		}
	}

	void LoadTexture() {
		if (gTileTex[static_cast<int>(TileType::TILE_BLOCK_GROUND_0)]) return;	

		gFallbackTex = AssetsManager::GetTexture("Assets/square.png");
		gFloorTopTex = AssetsManager::GetTexture(FLOOR_TOP_PATH);
		gFloorFillTex = AssetsManager::GetTexture(FLOOR_FILL_PATH);
		gTower1TopTex = AssetsManager::GetTexture(TOWER_BLOCK_TOP_1_PATH);
		gTower1BodyTex = AssetsManager::GetTexture(TOWER_BLOCK_1_PATH);
		gTower2TopTex = AssetsManager::GetTexture(TOWER_BLOCK_TOP_2_PATH);
		gTower2BodyTex = AssetsManager::GetTexture(TOWER_BLOCK_2_PATH);
		gCannonBaseTopTex = AssetsManager::GetTexture(CANNON_BASE_TOP_PATH);
		gCannonBaseTex = AssetsManager::GetTexture(CANNON_BASE_PATH);

		gCracksLightTex = AssetsManager::GetTexture(CRACKS_LIGHT_PATH);
		gCracksMediumTex = AssetsManager::GetTexture(CRACKS_MEDIUM_PATH);
		gCracksHeavyTex = AssetsManager::GetTexture(CRACKS_HEAVY_PATH);

		// Safety Fallback
		if (!gFloorTopTex)  gFloorTopTex = gFallbackTex;
		if (!gFloorFillTex) gFloorFillTex = gFallbackTex;
		if (!gTower1TopTex)  gTower1TopTex = gFallbackTex;
		if (!gTower1BodyTex)  gTower1BodyTex = gFallbackTex;
		if (!gTower2TopTex)      gTower2TopTex = gFallbackTex;
		if (!gTower2BodyTex)     gTower2BodyTex = gFallbackTex;
		if (!gCannonBaseTopTex)  gCannonBaseTopTex = gFallbackTex;
		if (!gCannonBaseTex)     gCannonBaseTex = gFallbackTex;

		gTileTex[static_cast<int>(TileType::TILE_EMPTY)] = nullptr;
		// ground uses top/fill selection in RenderMap (this just marks “loaded”)
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_GROUND_0)] = gFloorTopTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_GROUND_1)] = gFloorFillTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_GROUND_2)] = gFloorFillTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_CANNON_BASE_TOP)] = gCannonBaseTopTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_CANNON_BASE)] = gCannonBaseTex;

		// DO NOT overwrite these afterwards
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_1_TOP)] = gTower1TopTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_1)] = gTower1BodyTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_2_TOP)] = gTower2TopTex;
		gTileTex[static_cast<int>(TileType::TILE_BLOCK_2)] = gTower2BodyTex;

		// cannon base tile texture
		gTileTex[static_cast<int>(TileType::TILE_PLAYER_CANNON)] = gCannonBaseTopTex;
		gTileTex[static_cast<int>(TileType::TILE_ENEMY_CANNON)] = gCannonBaseTopTex;

		gTileTex[static_cast<int>(TileType::TILE_PLAYER_CHARACTER)] = gFallbackTex;
		gTileTex[static_cast<int>(TileType::TILE_ENEMY_MINION)] = gFallbackTex;
		gTileTex[static_cast<int>(TileType::TILE_ENEMY_BOSS)] = gFallbackTex;
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
		gCannonBaseTopTex = nullptr;
		gCannonBaseTex = nullptr;
		gCracksLightTex = nullptr;
		gCracksMediumTex = nullptr;
		gCracksHeavyTex = nullptr;

		for (int i = 0; i < static_cast<int>(TileType::TILE_COUNT); ++i) gTileTex[i] = nullptr;
	}

	void ReadMapData(std::string const& fileName, std::vector<std::vector<Tile>> &map) {
		std::string filePath = mapFilePath + fileName + ".txt";
		std::ifstream ifs{ filePath };
		if (!ifs) {
			std::cerr << "Error: Could not open map data file at " << filePath << std::endl;
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
						<< " at (" << x << "," << y << ") in " << filePath
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
				tile.tileId = { x,y };
				tile.Type(static_cast<TileType>(type));
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

		for (std::vector<Tile> const& row : map) {
			for (Tile const& tile : row) {
				if (tile.Type() == TileType::TILE_EMPTY) continue; // don't draw empty tiles
				if (!drawCharacters && tile.IsCharacter()) continue; // don't draw character tiles

				// Get texture
				AEGfxTexture* baseTex = gTileTex[static_cast<int>(tile.Type())];
				if (!baseTex) continue;

				AEMtx33 tileMat = GraphicsManager::CreateTransformMatrix(
					tile.transform.pos.x, tile.transform.pos.y,
					tile.transform.size.x / 2.0f, tile.transform.size.y / 2.0f
				);

				Color drawColor = tile.textureColor;
				float damageFrac = 0.0f;

				//Darken damaged destructible blocks so damage reads clearly even at a glance
				if (tile.IsBlocks()) {
					damageFrac = GetBlockDamageFraction(tile);
					if (damageFrac > 0.0f) {
						drawColor = BuildDamagedBlockColor(tile, damageFrac);
					}
				}

				GraphicsManager::DrawGameObject(baseTex, tileMat, drawColor);

				// Draw cracks texture
				if (tile.IsBlocks() && damageFrac > 0.0f) {
					AEGfxTexture* cracksTex = nullptr;
					float primaryAlpha = 0.0f;
					float secondaryAlpha = 0.0f;

					GetCrackVisualSettings(damageFrac, cracksTex, primaryAlpha, secondaryAlpha);

					if (cracksTex) {
						// Main crack pass
						GraphicsManager::DrawGameObject(cracksTex, tileMat, Color{ 1.0f, 1.0f, 1.0f, primaryAlpha });

						// Extra reinforcing pass for stronger visibility on a zoomed-out map
						if (secondaryAlpha > 0.0f) {
							GraphicsManager::DrawGameObject(cracksTex, tileMat, Color{ 1.0f, 1.0f, 1.0f, secondaryAlpha });
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

	AEGfxTexture* GetTileTexture(TileType type) {
		int idx = static_cast<int>(type);
		if (idx < 0 || idx >= static_cast<int>(TileType::TILE_COUNT)) return nullptr;
		return gTileTex[idx];
	}
}