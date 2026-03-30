#ifndef MAPMANAGER_HPP
#define MAPMANAGER_HPP

#include "pch.hpp"

#include "Tile.hpp"

// Contain enum TileType, class Tile
namespace MapManager {
	// pointer to the current level map data that is being drawn
	extern std::vector<std::vector<Tile>> *currentMap;
	static std::string const mapFilePath = "Assets/DataMap/";

	inline std::string MapFilePath() { return mapFilePath; }

	// Load all the textures required for the map
	void LoadTexture();
	void Unload();
	// fileName - ONLY fileName NO fileName.txt DOES NOT WORK
	// map - 2D vector of Tile type to store the data in
	void ReadMapData(std::string const& fileName, std::vector<std::vector<Tile>>& map);
	// Rendering of the map layout
	void DrawMap(std::vector<std::vector<Tile>> const& map, bool drawCharacters = true);
	Tile* FindTile(TileIndex const& tileId);
	AEGfxTexture* GetTileTexture(TileType type);
}

#endif // !MapManager_HPP
