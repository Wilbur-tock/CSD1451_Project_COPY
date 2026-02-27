#include "Credits.hpp"
#include "GraphicsManager.hpp"
#include "GameStateManager.hpp"
#include "AssetsManager.hpp"
#include "Utils.hpp"

namespace Credits {
    TextObject creditsText, hintText;

    // Background texture
    AEGfxTexture* bgTexture = nullptr;

    // Scroll offset for credits
    float scrollOffset = 0.0f;
    float scrollSpeed = 0.3f;  // Adjust this to change scroll speed

    void Load() {
        // Load background if you want one
        // bgTexture = AssetsManager::GetTexture("Assets/credits_background.png");
    }

    void Initialize() {
        // Reset scroll position
        scrollOffset = -1.0f;  // Start from bottom of screen

        creditsText = { 
            "GAME CREDITS\n\n"
            "=== Basement Dwellers ===\n\n"
            "Tech Lead\n"
            "Dayrius Goh\n\n"
            "Project Manager\n"
            "Justin Wong\n\n"
            "Audio Lead\n"
            "Bryan Lim\n\n"
            "Data Lead\n"
            "Wilbur Tock\n\n"
            "UI/UX Lead\n"
            "Ken Nixon\n\n\n"

            "Made with assets made by Kenney (www.kenney.nl)\n\n\n"

            //"Created by \n"
            //"DigiPen Institute of Technology Singapore\n"
            //"for the Alpha Engine\n\n"

            "=== Special Thanks ===\n\n"

            "Faculty and Advisors:\n"
            "Prof Gerald, Prof Tommy ,Prof Soroor\n\n"

            "President:\n"
            "Claude Comair\n\n"

            "Executives: \n"
            "Claude Comair, CHU Jason Yeu Tat, \n Michael GATS, TAN Chek Ming, \n "
            "Prasanna Kumar GHALI, Mandy WONG, Johnny DEEK\n\n"

            "All our playtesters\n"
            "and supporters\n\n\n"

            "=== Tools & Libraries ===\n\n"

            "Alpha Engine\n"
            "Created/distributed by Kenney (www.kenney.nl)\n"
            "Liberation Mono Font\n\n\n"

            "Thank you for playing!\n\n"

            "WWW.DIGIPEN.EDU\n"
            "All content @2025 DigiPen Institute of Technology Singapore.\nAll Rights Reserved\n\n"

            "Press M to return to menu",
        };

        hintText = { "M - Return to Menu  |  SPACE - Skip",
            0.7f,
            Color(0.6f, 0.6f, 0.6f, 1.0f),
            { -0.98f, -0.93f },
        };
    }

    void Update() {
        // Scroll credits upward
        scrollOffset += scrollSpeed * static_cast<float>(AEFrameRateControllerGetFrameTime());

        // Return to main menu with ESC or BACKSPACE
        if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_BACK)) {
            GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
        }

        // Skip to end of credits with SPACE
        if (AEInputCheckTriggered(AEVK_SPACE)) {
            scrollOffset += 1.0f;
        }

        // Optional: Loop credits or return to menu when finished
        if (scrollOffset > 3.0f) {  // Adjust based on your credits length
            // Option 1: Loop credits
            // scrollOffset = -1.0f;

            // Option 2: Return to menu
            GameStateManager::next = GameStateManager::GameState::MAIN_MENU;
        }

        Draw();
    }

    void Draw() {
        // Draw background
        if (bgTexture != nullptr) {
            GraphicsManager::DrawWorldUI(
                bgTexture,
                GraphicsManager::CreateTransformMatrix(
                    0, 0, 0,
                    Config::SCREEN_WIDTH,
                    Config::SCREEN_HEIGHT
                ),
                Color(1.0f, 1.0f, 1.0f, 1.0f)
            );
        }
        else {
            // Dark background
            AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
        }

        // Draw credits text with scroll offset
        creditsText.pos = { -0.3f, scrollOffset };
        creditsText.Draw();

        // Draw hint at 
        hintText.Draw();
    }

    void Free() {
        // Clean up when leaving credits
        scrollOffset = 0.0f;
    }

    void Unload() {
        // Unload resources if needed
    }
}