/*
Authors: Nixon
Description: A reusable modal confirmation dialog with YES and NO buttons.

All content © 2026 DigiPen Institute of Technology Singapore. All Rights Reserved
*/

#ifndef CONFIRMDIALOG_HPP
#define CONFIRMDIALOG_HPP

#include "pch.hpp"
#include "Utils.hpp"

namespace ConfirmDialog {

    // Show the dialog with a short description of the action and what will happen
    // when the player confirms. onConfirm is called only if YES is pressed.
    // Calling Open() while the dialog is already open replaces the pending action.
    void Open(const std::string& message, const std::string& subMessage, Utils::FuncPtr onConfirm);

    // Convenience overload: single-line message with no sub-message.
    void Open(const std::string& message, Utils::FuncPtr onConfirm);

    // Draw the dialog and handle YES/NO input. Call this every frame while IsOpen().
    void Update();

    // Returns true if the dialog is currently visible and waiting for player input.
    bool IsOpen();

    // Dismiss the dialog without firing the pending action (equivalent to pressing NO).
    void Close();

} // namespace ConfirmDialog

#endif // CONFIRMDIALOG_HPP