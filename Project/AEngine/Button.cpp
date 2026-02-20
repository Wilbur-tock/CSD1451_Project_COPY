#include "Button.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"
#include "GraphicsManager.hpp"

Button::Button(AEVec2 _pos, AEVec2 _size, Color _color, AEGfxTexture* _texture, Utils::FuncPtr _onClick)
    : pos{ _pos }, size{_size}, color{_color}, texture{_texture}, onClick{_onClick} {
}

Button::Button(TextObject _textObj, Utils::RectMode _rectMode, AEVec2 _pos, AEVec2 _size, Color _color, AEGfxTexture* _texture, Utils::FuncPtr _onClick)
    : textObj{_textObj}, pos{ _pos }, size{ _size }, color{ _color }, texture{ _texture }, onClick{ _onClick } {

    // Must always get the size of the string to be printed out
    const char* myText = _textObj.textContent.c_str();
    // Get the normalized text width and height
    f32 textW, textH;
    AEGfxGetPrintSize(AssetsManager::textFont, myText, textObj.textScale, &textW, &textH);

    float x = pos.x, y = pos.y, borderGap = 0.01f;

    switch (_rectMode) {
        case Utils::RECT_TOP_LEFT: {
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x + borderGap;
            textObj.pos.y = y - borderGap;
            break;
        }
        case Utils::RECT_TOP_CENTER: {
            x += (size.x / 2.0f);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - (textW / 2.0f);
            textObj.pos.y = y ;
            break;
        }
        case Utils::RECT_TOP_RIGHT: {
            x += size.x;
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - (textW) - borderGap;
            textObj.pos.y = y - borderGap;
            break;
        }
        case Utils::RECT_CENTER_LEFT: {
            y -= (size.y / 2.0f);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x + borderGap;
            textObj.pos.y = y + (textH / 2.0f);
            break;
        }
        case Utils::RECT_CENTER: {
            // Convert button position from corner rectmode to middle rectmode (world pos)
            // then convert to normalized position 
            // then minus half of the text width and height
            x += (size.x / 2.0f), y -= (size.y / 2.0f);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - (textW / 2.0f);
            textObj.pos.y = y + (textH / 2.0f);
            break;
        }
        case Utils::RECT_CENTER_RIGHT: {
            x += size.x, y -= (size.y / 2.0f);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - (textW) - borderGap;
            textObj.pos.y = y + (textH / 2.0f);
            break;
        }
        case Utils::RECT_BOTTOM_LEFT: {
            y -= (size.y);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x + borderGap;
            textObj.pos.y = y + textH + borderGap;
            break;
        }
        case Utils::RECT_BOTTOM_CENTER: {
            x += (size.x / 2.0f), y -= (size.y);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - (textW / 2.0f);
            textObj.pos.y = y + textH + borderGap;
            break;
        }
        case Utils::RECT_BOTTOM_RIGHT: {
            x += size.x, y -= (size.y);
            Utils::NormalizeCoordinates(x, y);
            textObj.pos.x = x - textW - borderGap;
            textObj.pos.y = y + textH + borderGap;
            break;
        }
    }
}

void Button::Update() {
     isHovered = CollisionManager::IsPointInRect(pos.x, pos.y, size.x, size.y);

     if (isHovered) {
         if (AEInputCheckTriggered(AEVK_LBUTTON) && onClick) {
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
    GraphicsManager::DrawUIFollowCam(texture, GraphicsManager::CreateTransformMatrix(pos.x, pos.y, 0, size.x, size.y), btnColor);

    // Draw text if the string is not a empty string
    if (!textObj.textContent.empty()) {
        GraphicsManager::DrawTextUI(textObj);
    }
}