#ifndef MAPEDITOR_HPP
#define MAPEDITOR_HPP

#include "pch.hpp"
#include "AEInput.h"
#include "MapManager.hpp"
#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "Utils.hpp"
#include "GameStateManager.hpp"

namespace MapEditor {
	void Init();
	// Map editor containing all the functions to edit the map and save/load files
	void MapEditor();
	void Exit();
}

#endif // !MAPEDITOR_HPP