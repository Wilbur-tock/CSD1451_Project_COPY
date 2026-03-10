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