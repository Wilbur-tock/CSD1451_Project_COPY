#ifndef LEVEL1_HPP
#define LEVEL1_HPP

#include "pch.hpp"

namespace Level1 {
	// Load resources required
	void Load();

	// Initalize level variables and settings
	void Init();

	// Update Logic
	void Update();

	// Render assets
	void Draw();

	// Frees memory or resources allocated
	void Free();

	// Cleans up resources loaded
	void Unload();
}

#endif // !LEVEL1_HPP
