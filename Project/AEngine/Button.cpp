#include "Button.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"
#include "GraphicsManager.hpp"
#include "AudioManager.hpp"

Button::Button(AEVec2 _pos, AEVec2 _size, Color _color, AEGfxTexture* _texture, Utils::FuncPtr _onClick)
    : textObj{ "" }, transform {_pos, _size}, color{_color}, texture{_texture}, onClick{_onClick} {}

Button::Button(TextObject _textObj, Utils::AnchorMode _anchorMode, AEVec2 _pos, AEVec2 _size, Color _color, AEGfxTexture* _texture, Utils::FuncPtr _onClick)
    : textObj{_textObj}, transform{ _pos, _size }, color{ _color }, texture{ _texture }, onClick{ _onClick } {
    // textObject is always in the center of the button by default
    textObj.SetPosition(_anchorMode, transform);
}

void Button::Update() {
    isClick = false;
    isHovered = CollisionManager::IsPointInRect(transform.pos.x, transform.pos.y, transform.size.x, transform.size.y);

    if (isHovered) {
        // Checking if user has click the button
        if (AEInputCheckTriggered(AEVK_LBUTTON) && onClick) {
            AudioManager::PlaySFX(AudioManager::SFX::UIButton);
            isClick = true;
            onClick();
        }
    }

    Draw();
}

void Button::Draw() {
    Color btnColor = color;
    if (isHovered) {
        btnColor = Color{ color.Red(), color.Green(), color.Blue(), color.Alpha() * 0.5f }; // Assign hovered color
    }

    // Draw button object
    GraphicsManager::DrawScreenOverlay(texture, GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, 0, transform.size.x, transform.size.y), btnColor);

    // Draw text if the string is not a empty string
    if (!textObj.TextContent().empty()) {
        textObj.Draw();
    }
}

void Button::SetTextOffset(float offsetX, float offsetY) {
    textObj.SetOffsetPosition(offsetX, offsetY);
}

void Button::SetPosition(Transform const& parent, Utils::AnchorMode mode) {
    // Find the anchor position
    Utils::SetAnchoredPosition(parent, transform, mode);
}