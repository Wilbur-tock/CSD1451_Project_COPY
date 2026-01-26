#ifndef MAPEDITOR_HPP
#define MAPEDITOR_HPP

#include "pch.hpp"

namespace MapEditor {
	void MapEditor();

	// Save files can be found in bin either debug/release in Assets/Map/ folder 
	void SaveMapFile(std::string filePath);
}

#endif // !MAPEDITOR_HPP