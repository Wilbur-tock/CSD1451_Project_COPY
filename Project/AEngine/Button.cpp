#include "Button.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"
#include "GraphicsManager.hpp"
#include "AudioManager.hpp"

Button::Button(AEVec2 _pos, AEVec2 _size, Color _color, AEGfxTexture* _bgTexture, AEGfxTexture* _iconTexture, Utils::FuncPtr _onClick)
    : textObj{ "" }, transform{ _pos, _size }, color{ _color }, bgTexture{ _bgTexture }, 
    iconTexture{ _iconTexture }, iconTransform{ {_pos}, {_size} }, onClick{ _onClick } {
    if (!iconTexture) return; // null icon texture don't need to calculate
    SetIconGaps(borderGaps);
}

Button::Button(TextObject _textObj, Utils::AnchorMode _anchorMode, AEVec2 _pos, AEVec2 _size, Color _color, 
    AEGfxTexture* _bgTexture, AEGfxTexture* _iconTexture,  Utils::FuncPtr _onClick)
    : textObj{ _textObj }, transform{ _pos, _size }, color{ _color },
    bgTexture{ _bgTexture }, iconTexture{ _iconTexture }, iconTransform{ {_pos}, {_size} }, onClick{ _onClick } {
    // textObject is always in the center of the button by default
    textObj.SetPosition(_anchorMode, transform);
    if (!iconTexture) return; // null icon texture don't need to calculate
    SetIconGaps(borderGaps);
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
    Color iconColor{ 1,1,1 };
    if (isHovered) {
        // Assign hovered color
        float alpha = 0.3f;
        btnColor = Color{ color.Red(), color.Green(), color.Blue(), color.Alpha() * alpha };
        iconColor = Color{ iconColor.Red(), iconColor.Green(), iconColor.Blue(), iconColor.Alpha() * alpha };
    }

    // Draw button background
    GraphicsManager::DrawScreenOverlay(bgTexture, GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, 0, transform.size.x, transform.size.y), btnColor);

    // Draw text if the string is not a empty string
    if (!textObj.TextContent().empty()) {
        textObj.Draw();
    }

    // Draw icon if there is onee
    if (iconTexture) {
        GraphicsManager::DrawScreenOverlay(iconTexture, GraphicsManager::CreateTransformMatrix(iconTransform.pos.x, iconTransform.pos.y, 0, iconTransform.size.x, iconTransform.size.y), iconColor);
    }
}

void Button::SetTextOffset(float offsetX, float offsetY) {
    textObj.SetOffsetPosition(offsetX, offsetY);
}

void Button::SetPosition(Transform const& parent, Utils::AnchorMode mode) {
    // Find the anchor position
    Utils::SetAnchoredPosition(parent, transform, mode);
}

void Button::SetIconGaps(BorderGaps gaps) {
    borderGaps = gaps;
    // Change top-left corner anchor position
    iconTransform.pos.x = transform.pos.x + borderGaps.xLeft;
    iconTransform.pos.y = transform.pos.y - borderGaps.yTop; // - because y-axis is 0 to max from top to bottom
    // Change width and height based on gaps between left, right, top and bottom
    iconTransform.size.x = transform.size.x - (borderGaps.xLeft + borderGaps.xRight);
    iconTransform.size.y = transform.size.y - (borderGaps.yTop + borderGaps.yBottom);
}