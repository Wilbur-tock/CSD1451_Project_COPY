#include "Button.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"
#include "GraphicsManager.hpp"
#include "AudioManager.hpp"

Button::Button(AEVec2 _pos, AEVec2 _size, Color _color, AEGfxTexture* _texture, Utils::FuncPtr _onClick)
    : textObj{ "" }, transform {_pos, _size}, color{_color}, texture{_texture}, onClick{_onClick} {}

Button::Button(TextObject _textObj, Utils::AnchorMode _anchorMode, AEVec2 _pos, AEVec2 _size, Color _color, AEGfxTexture* _texture, Utils::FuncPtr _onClick)
    : textObj{_textObj}, transform{ _pos, _size }, color{ _color }, texture{ _texture }, onClick{ _onClick } {

    // Must always get the size of the string to be printed out
    const char* myText = _textObj.textContent.c_str();
    // Get the normalized text width and height
    f32 textW, textH;
    AEGfxGetPrintSize(AssetsManager::textFont, myText, textObj.textScale, &textW, &textH);

    float x = transform.pos.x, y = transform.pos.y, borderGap = 0.01f;
    textObjAnchorMode = _anchorMode;

    switch (_anchorMode) {
        case Utils::AnchorMode::TOP_LEFT: {
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x + borderGap;
            textObj.pos.y = y - borderGap;
            break;
        }
        case Utils::AnchorMode::TOP_CENTER: {
            x += (transform.size.x / 2.0f);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - (textW / 2.0f);
            textObj.pos.y = y ;
            break;
        }
        case Utils::AnchorMode::TOP_RIGHT: {
            x += transform.size.x;
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - (textW) - borderGap;
            textObj.pos.y = y - borderGap;
            break;
        }
        case Utils::AnchorMode::CENTER_LEFT: {
            y -= (transform.size.y / 2.0f);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x + borderGap;
            textObj.pos.y = y + (textH / 2.0f);
            break;
        }
        case Utils::AnchorMode::CENTER: {
            // Convert button position from corner rectmode to middle rectmode (world pos)
            // then convert to normalized position 
            // then minus half of the text width and height
            x += (transform.size.x / 2.0f), y -= (transform.size.y / 2.0f);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - (textW / 2.0f);
            textObj.pos.y = y + (textH / 2.0f);
            break;
        }
        case Utils::AnchorMode::CENTER_RIGHT: {
            x += transform.size.x, y -= (transform.size.y / 2.0f);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - (textW) - borderGap;
            textObj.pos.y = y + (textH / 2.0f);
            break;
        }
        case Utils::AnchorMode::BOTTOM_LEFT: {
            y -= (transform.size.y);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x + borderGap;
            textObj.pos.y = y + textH + borderGap;
            break;
        }
        case Utils::AnchorMode::BOTTOM_CENTER: {
            x += (transform.size.x / 2.0f), y -= (transform.size.y);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - (textW / 2.0f);
            textObj.pos.y = y + textH + borderGap;
            break;
        }
        case Utils::AnchorMode::BOTTOM_RIGHT: {
            x += transform.size.x, y -= (transform.size.y);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - textW - borderGap;
            textObj.pos.y = y + textH + borderGap;
            break;
        }
    }
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
    if (!textObj.textContent.empty()) {
        textObj.Draw();
    }
}

void Button::SetTextOffset(f32 offsetX, f32 offsetY) {
    textObj.pos.x += offsetX;
    textObj.pos.y += offsetY;
}

void Button::SetTextScale(float scale) {
    textObj.textScale = scale;
    RecalculateTextPos();
}

void Button::SetAsChildObj(Transform const& parent, Utils::AnchorMode mode) {
    // Find the anchor position
    Utils::SetAnchoredPosition(parent, transform, mode);
}

void Button::RecalculateTextPos() {
    const char* myText = textObj.textContent.c_str();
    f32 textW, textH;
    AEGfxGetPrintSize(AssetsManager::textFont, myText, textObj.textScale, &textW, &textH);

    float x = transform.pos.x, y = transform.pos.y, borderGap = 0.01f;

    switch (textObjAnchorMode) {
    case Utils::AnchorMode::TOP_LEFT: {
        Utils::NormalizeCoordinates(x, y);
        textObj.pos.x = x + borderGap;
        textObj.pos.y = y - borderGap;
        break;
    }
    case Utils::AnchorMode::TOP_CENTER: {
        x += (transform.size.x / 2.0f);
        Utils::NormalizeCoordinates(x, y);
        textObj.pos.x = x - (textW / 2.0f);
        textObj.pos.y = y;
        break;
    }
    case Utils::AnchorMode::TOP_RIGHT: {
        x += transform.size.x;
        Utils::NormalizeCoordinates(x, y);
        textObj.pos.x = x - (textW)-borderGap;
        textObj.pos.y = y - borderGap;
        break;
    }
    case Utils::AnchorMode::CENTER_LEFT: {
        y -= (transform.size.y / 2.0f);
        Utils::NormalizeCoordinates(x, y);
        textObj.pos.x = x + borderGap;
        textObj.pos.y = y + (textH / 2.0f);
        break;
    }
    case Utils::AnchorMode::CENTER: {
        x += (transform.size.x / 2.0f), y -= (transform.size.y / 2.0f);
        Utils::NormalizeCoordinates(x, y);
        textObj.pos.x = x - (textW / 2.0f);
        textObj.pos.y = y + (textH / 2.0f);
        break;
    }
    case Utils::AnchorMode::CENTER_RIGHT: {
        x += transform.size.x, y -= (transform.size.y / 2.0f);
        Utils::NormalizeCoordinates(x, y);
        textObj.pos.x = x - (textW)-borderGap;
        textObj.pos.y = y + (textH / 2.0f);
        break;
    }
    case Utils::AnchorMode::BOTTOM_LEFT: {
        y -= (transform.size.y);
        Utils::NormalizeCoordinates(x, y);
        textObj.pos.x = x + borderGap;
        textObj.pos.y = y + textH + borderGap;
        break;
    }
    case Utils::AnchorMode::BOTTOM_CENTER: {
        x += (transform.size.x / 2.0f), y -= (transform.size.y);
        Utils::NormalizeCoordinates(x, y);
        textObj.pos.x = x - (textW / 2.0f);
        textObj.pos.y = y + textH + borderGap;
        break;
    }
    case Utils::AnchorMode::BOTTOM_RIGHT: {
        x += transform.size.x, y -= (transform.size.y);
        Utils::NormalizeCoordinates(x, y);
        textObj.pos.x = x - textW - borderGap;
        textObj.pos.y = y + textH + borderGap;
        break;
    }
    }
}