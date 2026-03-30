/*
Authors: Bryan, Dayrius
Description: Display the level selection page

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef LEVEL_SELECT_HPP
#define LEVEL_SELECT_HPP

namespace LevelSelect {
    void Load();
    void Initialize();
    void Update();
    void Draw();
    void Free();
    void Unload();

    void OnBackClicked();
}

#endif // LEVEL_SELECT_HPP