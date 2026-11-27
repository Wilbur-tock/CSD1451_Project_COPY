# Fortress Fight
A turn-based artillery game built in C++ by a team of 5 students at DigiPen Institute of Technology Singapore. Players take turns firing projectiles at each other's fortresses, using abilities to boost their attacks or strengthen their defences.

## My contribution — Wilbur Tock

I implemented the game's **JSON data loader** in C++ using [nlohmann/json](https://github.com/nlohmann/json). It reads all game data from JSON files into C++ objects at startup, so the team could add or tune content without touching code.

This included the **data-driven ability system**, which has 3 ability types:


| Offensive | Empowers the player's projectiles |
| Defensive | Gives the fortress specific defensive capabilities |
| Stat buff | Raises raw HP, attack or defence |

## Built with

C++, nlohmann/json, [graphics library/engine], [build tool, e.g. CMake / Visual Studio]

## How to build and run

1. open the .sln in Visual Studio 2022
2. press the build button at the top
3. play the game
