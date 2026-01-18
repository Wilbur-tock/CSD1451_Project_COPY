#ifndef MAPEDITOR_HPP
#define MAPEDITOR_HPP

namespace MapEditor {
	void MapEditor();

	/* Save files can be found in bin either debug/release in Assets/Map/ folder */
	void SaveMapFile(const char* filePath);
}

#endif // !MAPEDITOR_HPP