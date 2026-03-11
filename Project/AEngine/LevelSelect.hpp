#ifndef LEVEL_SELECT_HPP
#define LEVEL_SELECT_HPP

#include "pch.hpp"
#include "Button.hpp"

namespace LevelSelect {
    void Load();
    void Initialize();
    void Update();
    void Draw();
    void Free();
    void Unload();

    void OnLevel1Clicked();
    void OnLevel2Clicked();
    void OnBackClicked();
}

#endif // LEVEL_SELECT_HPP