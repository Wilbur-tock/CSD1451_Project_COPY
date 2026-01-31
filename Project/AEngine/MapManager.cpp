#include "pch.hpp"
#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Color.hpp"

namespace MapManager {
	std::vector<std::vector<Tile>> mapLayout(Config::GRID_HEIGHT, std::vector<Tile>(Config::GRID_WIDTH));

	static const char* FLOOR_TOP_PATH = "Assets/floorTile.png";  // surface layer floor
	static const char* FLOOR_FILL_PATH = "Assets/floorFill.png";  // underground fill of floor
	static const char* TOWER_BLOCK_1_PATH = "Assets/towerBlock1.png";
	static const char* TOWER_BLOCK_2_PATH = "Assets/towerBlock2.png";
	static const char* CANNON_BASE_PATH = "Assets/cannonBase.png"; //Cannon (for testing now, not actual sprite)

	Tile::Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type) 
		: GameObject{ _pos, {size,size}, _texture, Color{1,1,1} }, type{ _type } {
	}

	// Cached textures
	static AEGfxTexture* gTileTex[MapManager::TILE_COUNT] = { nullptr };
	static AEGfxTexture* gFloorTopTex = nullptr;
	static AEGfxTexture* gFloorFillTex = nullptr;
	static AEGfxTexture* gTower1Tex = nullptr;
	static AEGfxTexture* gTower2Tex = nullptr;
	static AEGfxTexture* gCannonBaseTex = nullptr;
	static AEGfxTexture* gFallbackTex = nullptr;

	static void EnsureTileTexturesLoaded()
	{
		if (gTileTex[MapManager::TILE_BLOCK_GROUND]) return;

		gFallbackTex = AssetsManager::GetTexture("Assets/square.png");
		gFloorTopTex = AssetsManager::GetTexture(FLOOR_TOP_PATH);
		gFloorFillTex = AssetsManager::GetTexture(FLOOR_FILL_PATH);
		gTower1Tex = AssetsManager::GetTexture(TOWER_BLOCK_1_PATH);
		gTower2Tex = AssetsManager::GetTexture(TOWER_BLOCK_2_PATH);
		gCannonBaseTex = AssetsManager::GetTexture(CANNON_BASE_PATH);

		// Safety Fallback
		if (!gFloorTopTex)  gFloorTopTex = gFallbackTex;
		if (!gFloorFillTex) gFloorFillTex = gFallbackTex;
		if (!gTower1Tex)     gTower1Tex = gFallbackTex;
		if (!gTower2Tex)     gTower2Tex = gFallbackTex;
		if (!gCannonBaseTex) gCannonBaseTex = gFallbackTex;

		gTileTex[MapManager::TILE_EMPTY] = nullptr;
		// ground uses top/fill selection in RenderMap (this just marks “loaded”)
		gTileTex[MapManager::TILE_BLOCK_GROUND] = gFloorTopTex;

		// ✅ DO NOT overwrite these afterwards
		gTileTex[MapManager::TILE_BLOCK_1] = gTower1Tex;
		gTileTex[MapManager::TILE_BLOCK_2] = gTower2Tex;

		// cannon base tile texture
		gTileTex[MapManager::TILE_PLAYER_CANNON] = gCannonBaseTex;

		// other types can stay fallback for now
		gTileTex[MapManager::TILE_PLAYER_CHARACTER] = gFallbackTex;
		gTileTex[MapManager::TILE_ENEMY_MINION] = gFallbackTex;
		gTileTex[MapManager::TILE_ENEMY_BOSS] = gFallbackTex;

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
		if (gTower1Tex == gFallbackTex)
			std::cerr << "[MapManager] WARNING: missing towerBlock1 at " 
			<< TOWER_BLOCK_1_PATH << "\n";
		if (gTower2Tex == gFallbackTex)
			std::cerr << "[MapManager] WARNING: missing towerBlock2 at " 
			<< TOWER_BLOCK_2_PATH << "\n";
		if (gCannonBaseTex == gFallbackTex)
			std::cerr << "[MapManager] WARNING: missing cannonBase at " 
			<< CANNON_BASE_PATH << "\n";
	}

	static AEGfxTexture* TextureForTile(MapManager::TileType t)
	{
		EnsureTileTexturesLoaded();
		if (t < 0 || t >= MapManager::TILE_COUNT) return nullptr;
		return gTileTex[t];
	}

	std::string TileTypeToString(int id) {
		if (id < 0 || id >= static_cast<int>(tileNames.size()))
			return "INVALID";
		return tileNames[id];
	}

	void ReadMapData(std::string fileName, std::vector<std::vector<Tile>> &map) {
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

				if (type < 0 || type >= MapManager::TILE_COUNT) {
					std::cerr << "[MapManager] WARNING: invalid tile id " << type
						<< " at (" << x << "," << y << ") in " << fileName
						<< " -> forcing TILE_EMPTY\n";
					type = MapManager::TILE_EMPTY;
				}

				map[y][x] = Tile({ startX + (x * tileSize), startY - (y * tileSize) }, tileTexture, static_cast<TileType>(type));
			}
		}

		ifs.close();
	}


	void RenderMap(const std::vector<std::vector<Tile>>& map) {
		EnsureTileTexturesLoaded();

		float tileSize = static_cast<float>(Config::TILE_SIZE);

		// For each column x, find the first (topmost) ground tile y
		std::vector<int> surfaceY(Config::GRID_WIDTH, -1);
		for (int x = 0; x < Config::GRID_WIDTH; ++x) {
			for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
				TileType t = map[y][x].type;
				if (t == TILE_BLOCK_GROUND || t == TILE_PLAYER_CANNON) {
					surfaceY[x] = y;
					break;
				}
			}
		}

		for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
			for (int x = 0; x < Config::GRID_WIDTH; ++x) {

				const Tile& tile = map[y][x];

				if (tile.type == TILE_EMPTY)
					continue;

				AEMtx33 transform =
					GraphicsManager::CreateTransformMatrix(tile.pos.x, tile.pos.y, 0.0f,
						tileSize / 2.0f, tileSize / 2.0f);

				if (tile.type == TILE_BLOCK_GROUND) {
					bool isSurface = (surfaceY[x] == y);
					AEGfxTexture* floorTex = isSurface ? gFloorTopTex : gFloorFillTex;
					GraphicsManager::DrawGameObject(floorTex, transform, Color{ 1,1,1,1 });
					continue;
				}
				if (tile.type == TILE_PLAYER_CANNON) {
					bool isSurface = (surfaceY[x] == y);
					AEGfxTexture* under = isSurface ? gFloorTopTex : gFloorFillTex;

					// 1) draw floor underneath (keeps the grass strip continuous)
					GraphicsManager::DrawGameObject(under, transform, Color{ 1,1,1,1 });
					// 2) draw cannon base on top (so it no longer looks like floor)
					GraphicsManager::DrawGameObject(gCannonBaseTex, transform, Color{ 1,1,1,1 });
					continue;
				}

				AEGfxTexture* tex = TextureForTile(tile.type);
				if (!tex) tex = gFallbackTex;

				GraphicsManager::DrawGameObject(tex, transform, Color{ 1,1,1,1 });
			}
		}
	}
}