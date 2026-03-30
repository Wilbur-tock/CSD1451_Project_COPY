/*
Authors: Bryan
Description: Display the game guide information

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef GUIDE_HPP
#define GUIDE_HPP

#include "pch.hpp"
#include "Button.hpp"

namespace Guide {
    void Load();
    void Initialize();
    void Update();
    void Draw();
    void Free();
    void Unload();

    void OnBackClicked();
    void OnGameGuideClicked();
    void OnAbilitiesGuideClicked();
}

#endif // GUIDE_HPP