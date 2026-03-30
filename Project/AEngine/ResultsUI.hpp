/*
Authors: Nixon
Description: Manage and draw the leve game over results UI

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef RESULTSUI_HPP
#define RESULTSUI_HPP

#include "LevelManager.hpp"

namespace ResultsUI {
	void Intialize();
	void Update(LevelStatus status);
	void Draw(LevelStatus status);
}

#endif // !RESULTSUI_HPP
