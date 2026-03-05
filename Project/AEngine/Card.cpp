#include "Card.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"
#include "GraphicsManager.hpp"

Card::Card(AEVec2 _pos, AEVec2 _size, Color _color, int abilityId, Utils::FuncPtr _onClick)
    : pos{ _pos }, size{ _size }, color{ _color }, onClick{ _onClick } {
}

Card::Card(TextObject _textObj, AEVec2 _pos, AEVec2 _size, Color _color,
    int abilityId, Utils::FuncPtr _onClick)
    : textObj{ _textObj }, pos{ _pos }, size{ _size }, color{ _color }, onClick{ _onClick } {
    
}

void Card::Update() {
    isHovered = CollisionManager::IsPointInRect(pos.x, pos.y, size.x, size.y);

    if (isHovered) {
        if (AEInputCheckTriggered(AEVK_LBUTTON) && onClick) {
            onClick();
        }
    }

    Draw();
}

void Card::Draw() {
    Color cardColor = color;
    if (isHovered) {
        cardColor = Color{ color.Red(), color.Green(), color.Blue(), color.Alpha() * 0.5f }; // Assign hovered color
    }

    // Draw card object
    GraphicsManager::DrawScreenOverlay(cardTexture, GraphicsManager::CreateTransformMatrix(pos.x, pos.y, 0, size.x, size.y), cardColor);
       
    // Draw text if the string is not a empty string
    if (!textObj.textContent.empty()) {
        textObj.Draw();
    }
}