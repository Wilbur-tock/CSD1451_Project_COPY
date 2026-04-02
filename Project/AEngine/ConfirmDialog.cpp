/*
Authors: Nixon
Description: A reusable modal confirmation dialog with YES and NO buttons.

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#include "ConfirmDialog.hpp"
#include "Button.hpp"
#include "UIManager.hpp"
#include "AssetsManager.hpp"
#include "Config.hpp"
#include "Color.hpp"
#include "AEEngine.h"

namespace ConfirmDialog {

    // ---------------------------------------------------------------------------
    // Internal state
    // ---------------------------------------------------------------------------

    namespace {

        bool isOpen = false; // true while the dialog is waiting for input
        bool isInitialized = false; // lazy-init guard

        Utils::FuncPtr pendingAction; // callback fired when YES is pressed
        std::string    currentMessage;    // primary line shown inside the panel
        std::string    currentSubMessage; // secondary smaller line (may be empty)

        // Buttons
        Button yesButton;
        Button noButton;

        // ---------------------------------------------------------------------------
        // Callbacks
        // ---------------------------------------------------------------------------

        void OnYesClicked() {
            isOpen = false;                 // dismiss dialog first so callers
            if (pendingAction) {            // are not blocked when the callback
                pendingAction();            // performs a state transition
            }
        }

        void OnNoClicked() {
            isOpen = false; // discard the pending action; resume normal flow
        }

        // ---------------------------------------------------------------------------
        // Lazy initialization
        //
        // Called the first time Open() is used. At that point AssetsManager already
        // has the button texture cached (MainMenu::Load runs before any confirm
        // dialog can appear).
        // ---------------------------------------------------------------------------
        void Init() {
            if (isInitialized) return;

            AEGfxTexture* btnTex = AssetsManager::GetTexture("Assets/buttonTexture.png");
            if (!btnTex) btnTex = AssetsManager::squareTexture; // safe fallback

            // -----------------------------------------------------------------------
            // Button layout
            //
            // The dialog panel is 400 x 180, centered at world origin (0, 0).
            //   Panel X range: -200 .. 200   Panel Y range: -90 .. 90
            //
            // Two buttons sit side by side in the lower half of the panel.
            //   btnW=150  btnH=55  gap=20
            //   Total row width = 150 + 20 + 150 = 320
            //   Row is centered -> left edge at X = -160
            //
            //   NO  button top-left: (-160, -30)   (left, safe action  -> green)
            //   YES button top-left: (  10, -30)   (right, danger action -> red)
            //
            // Putting NO on the left and YES on the right makes the safe choice the
            // first one the mouse reaches from the center, reducing accidental confirms.
            // -----------------------------------------------------------------------

            const float btnW = 150.0f;
            const float btnH = 55.0f;
            const float gap = 20.0f;
            const float btnY = -30.0f; // top-left Y of both buttons

            const float totalRowW = btnW * 2.0f + gap;
            const float rowLeft = -totalRowW * 0.5f; // centers the row on X=0

            const float noX = rowLeft;             // NO  is on the LEFT  (safe)
            const float yesX = rowLeft + btnW + gap; // YES is on the RIGHT (danger)

            noButton = Button(
                TextObject{ "NO" },
                Utils::AnchorMode::CENTER,
                { noX, btnY },
                { btnW, btnH },
                Color(0.2f, 0.7f, 0.3f, 1.0f), // green -- safe / cancel
                btnTex,
                nullptr,
                OnNoClicked
            );

            yesButton = Button(
                TextObject{ "YES" },
                Utils::AnchorMode::CENTER,
                { yesX, btnY },
                { btnW, btnH },
                Color(0.8f, 0.2f, 0.2f, 1.0f), // red -- confirm / danger
                btnTex,
                nullptr,
                OnYesClicked
            );

            isInitialized = true;
        }

    } // anonymous namespace

    // ---------------------------------------------------------------------------
    // Public API
    // ---------------------------------------------------------------------------

    void Open(const std::string& message, const std::string& subMessage, Utils::FuncPtr onConfirm) {
        Init(); // no-op after first call
        currentMessage = message;
        currentSubMessage = subMessage;
        pendingAction = onConfirm;
        isOpen = true;
    }

    void Open(const std::string& message, Utils::FuncPtr onConfirm) {
        Open(message, "", onConfirm); // delegate to the two-line overload
    }

    bool IsOpen() {
        return isOpen;
    }

    void Close() {
        isOpen = false; // discard pending action (same as pressing NO)
    }

    // ---------------------------------------------------------------------------
    // Update / Draw
    //
    // Must be called every frame while IsOpen() is true. Renders the full-screen
    // dim overlay, the dialog panel, both message lines, and the YES / NO buttons.
    // When YES is pressed the pending callback fires and isOpen is cleared.
    // ---------------------------------------------------------------------------
    void Update() {
        if (!isOpen) return;

        // -----------------------------------------------------------------------
        // Layer 1: full-screen dim overlay so the background is still readable
        // but visually de-emphasised. Using 70% opacity (slightly darker than
        // PauseMenu's 60%) to make the small dialog panel stand out clearly.
        // -----------------------------------------------------------------------
        UIManager::Panel dimOverlay{
            { static_cast<float>(Config::SCREEN_WIDTH),
              static_cast<float>(Config::SCREEN_HEIGHT) },
            Color(0.0f, 0.0f, 0.0f, 0.70f),
            Utils::AnchorMode::CENTER
        };
        dimOverlay.Draw();

        // -----------------------------------------------------------------------
        // Layer 2: dialog box -- a dark, slightly translucent panel centered on
        // the screen. Width 400 and height 180 comfortably fits two text lines
        // and two side-by-side buttons.
        // -----------------------------------------------------------------------
        UIManager::Panel dialogBox{
            { 400.0f, 180.0f },
            Color(0.10f, 0.10f, 0.16f, 0.96f),
            Utils::AnchorMode::CENTER
        };
        dialogBox.Draw();

        // -----------------------------------------------------------------------
        // Thin accent bar at the top of the dialog to visually separate the
        // title area from the body. Same red as the YES button to reinforce the
        // "danger" signal.
        // -----------------------------------------------------------------------
        UIManager::Panel accentBar{
            { 400.0f, 6.0f },
            Color(0.8f, 0.2f, 0.2f, 1.0f),
            Utils::AnchorMode::CENTER
        };
        // Shift bar to sit at the top of the dialog panel (panel top = +90, bar height = 6)
        accentBar.AddOffsetPosition({ 0.0f, 87.0f });
        accentBar.Draw();

        // -----------------------------------------------------------------------
        // Text: primary message (main question) and optional sub-message.
        //
        // AEGfxPrint uses normalized screen coordinates:
        //   X: -1.0 (left edge) .. +1.0 (right edge)
        //   Y: -1.0 (bottom)    .. +1.0 (top)
        // The screen center is (0, 0).
        //
        // Positions chosen to sit comfortably inside the 400x180 panel:
        //   "ARE YOU SURE?"  large,  slightly above center
        //   sub-message      smaller, just below it
        // -----------------------------------------------------------------------
        AEGfxPrint(
            AssetsManager::textFont,
            "ARE YOU SURE?",
            -0.10f, 0.10f,  // normalized position: near top-center of dialog
            0.75f, 0.75f,    // scale
            0.9f, 0.9f, 0.9f //  light grey -- secondary information
        );

        if (!currentMessage.empty()) {
            AEGfxPrint(
                AssetsManager::textFont,
                currentMessage.c_str(),
                -0.20f, 0.03f,  // one line below the title
                0.65f, 0.65f,
                1.0f, 0.9f, 0.5f // warm yellow -- draws the eye to the statement   
            );
        }

        if (!currentSubMessage.empty()) {
            AEGfxPrint(
                AssetsManager::textFont,
                currentSubMessage.c_str(),
                -0.20f, -0.03f, // another line below
                0.60f, 0.60f,
                0.7f, 0.7f, 0.7f // dimmer grey -- tertiary information
            );
        }

        // -----------------------------------------------------------------------
        // Buttons: NO on the left (safe / green), YES on the right (danger / red).
        //
        // Button::Update() handles hover detection, draw, and click firing.
        // Calling Update() instead of Draw() ensures clicks are processed.
        // -----------------------------------------------------------------------
        noButton.Update();
        yesButton.Update();
    }

} // namespace ConfirmDialog