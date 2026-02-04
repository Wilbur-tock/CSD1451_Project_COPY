#include "pch.hpp"
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

	Tile::Tile(AEVec2 _pos, AEGfxTexture* _texture, TileType _type) 
		: GameObject{ _pos, {defaultSize,defaultSize}, _texture, Color{1,1,1} }, type{ _type }, gridId{ 0,0 } {
	}

	void Tile::Damage(float damage) {
		health -= damage;
		if (health <= minHealth) {
			type = TILE_EMPTY; //TEMPORARY
		}
		//std::cout << "Damaged Tile x: " << id.x << " y: " << id.y << " health: " << health << std::endl;
	}

	float Tile::GetHealth() const {
		return health;
	}

	bool Tile::IsGround() const {
		return type >= TILE_BLOCK_GROUND_0 && type <= TILE_BLOCK_GROUND_2;
	}
	bool Tile::IsBlocks() const {
		return type == TILE_BLOCK_1
			|| type == TILE_BLOCK_2
			|| type == TILE_BLOCK_1_TOP
			|| type == TILE_BLOCK_2_TOP;
	}

	// Cached textures
	static AEGfxTexture* gTileTex[MapManager::TILE_COUNT] = { nullptr };
	static AEGfxTexture* gFloorTopTex = nullptr;
	static AEGfxTexture* gFloorFillTex = nullptr;
	static AEGfxTexture* gTower1TopTex = nullptr;
	static AEGfxTexture* gTower1BodyTex = nullptr;
	static AEGfxTexture* gTower2TopTex = nullptr;
	static AEGfxTexture* gTower2BodyTex = nullptr;
	static AEGfxTexture* gCannonBaseTex = nullptr;
	static AEGfxTexture* gFallbackTex = nullptr;

	void Load() {
		if (gTileTex[MapManager::TILE_BLOCK_GROUND_0]) return;	

		gFallbackTex = AssetsManager::GetTexture("Assets/square.png");
		gFloorTopTex = AssetsManager::GetTexture(FLOOR_TOP_PATH);
		gFloorFillTex = AssetsManager::GetTexture(FLOOR_FILL_PATH);
		gTower1TopTex = AssetsManager::GetTexture(TOWER_BLOCK_TOP_1_PATH);
		gTower1BodyTex = AssetsManager::GetTexture(TOWER_BLOCK_1_PATH);
		gTower2TopTex = AssetsManager::GetTexture(TOWER_BLOCK_TOP_2_PATH);
		gTower2BodyTex = AssetsManager::GetTexture(TOWER_BLOCK_2_PATH);
		gCannonBaseTex = AssetsManager::GetTexture(CANNON_BASE_PATH);

		// Safety Fallback
		if (!gFloorTopTex)  gFloorTopTex = gFallbackTex;
		if (!gFloorFillTex) gFloorFillTex = gFallbackTex;
		if (!gTower1TopTex)  gTower1TopTex = gFallbackTex;
		if (!gTower1BodyTex)  gTower1BodyTex = gFallbackTex;
		if (!gTower2TopTex)  gTower2TopTex = gFallbackTex;
		if (!gTower2BodyTex)  gTower2BodyTex = gFallbackTex;
		if (!gCannonBaseTex) gCannonBaseTex = gFallbackTex;

		gTileTex[MapManager::TILE_EMPTY] = nullptr;
		// ground uses top/fill selection in RenderMap (this just marks “loaded”)
		gTileTex[MapManager::TILE_BLOCK_GROUND_0] = gFloorTopTex;
		gTileTex[MapManager::TILE_BLOCK_GROUND_1] = gFloorFillTex;
		gTileTex[MapManager::TILE_BLOCK_GROUND_2] = gFloorFillTex;

		// DO NOT overwrite these afterwards
		gTileTex[MapManager::TILE_BLOCK_1_TOP] = gTower1TopTex;
		gTileTex[MapManager::TILE_BLOCK_1] = gTower1BodyTex;
		gTileTex[MapManager::TILE_BLOCK_2_TOP] = gTower2TopTex;
		gTileTex[MapManager::TILE_BLOCK_2] = gTower2BodyTex;

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
	}

	std::string TileTypeToString(int id) {
		if (id < 0 || id >= static_cast<int>(tileNames.size())) return "INVALID";
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
				else {
					tileTexture = gTileTex[type];
				}

				Tile& tile = map[y][x];
				tile.pos = { startX + (x * tileSize), startY - (y * tileSize) };
				tile.size = { Config::TILE_SIZE,Config::TILE_SIZE };
				tile.texture = tileTexture;
				tile.type = static_cast<TileType>(type);
				tile.gridId.x = static_cast<f32>(x);
				tile.gridId.y = static_cast<f32>(y);
			}
		}

		currentMap = &map;

		ifs.close();
	}

	void RenderMap(const std::vector<std::vector<Tile>>& map) {
		for (int y = 0; y < Config::GRID_HEIGHT; ++y) {
			for (int x = 0; x < Config::GRID_WIDTH; ++x) {
				const Tile& tile = map[y][x];
				if (tile.type == TILE_EMPTY) continue;
				AEMtx33 transform = GraphicsManager::CreateTransformMatrix(tile.pos.x, tile.pos.y, 0.0f, tile.size.x / 2.0f, tile.size.y / 2.0f);
				GraphicsManager::DrawGameObject(gTileTex[tile.type], transform, Color{1,1,1,1});
			}
		}
	}
}