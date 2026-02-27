#include "UIManager.hpp"

#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"

namespace UIManager {
    Bar::Bar(AEVec2 _pos, AEVec2 _size, Color _color, FillType _type)
        : transform{ _pos, _size }, color{ _color }, type{ _type } {}

	void Bar::Draw(float currValue, float maxValue) {
        AEGfxTexture* tex = AssetsManager::squareTexture;

        float x = transform.pos.x, y = transform.pos.y;
        float w = transform.size.x, h = transform.size.y;

        float halfWidth = transform.size.x / 2.0f;
        x -= halfWidth;

        // Lighter background bar
        Color bgColor = { color.Red(), color.Green(), color.Blue() };
        bgColor.EditColorPercentage(0.8f);
        GraphicsManager::DrawWorldUI(tex, GraphicsManager::CreateTransformMatrix(x, y, 0, w, h), bgColor);

        float percentage = (currValue / maxValue);
        float newW = w, newH = h;
        if (type == FillType::RIGHT_TO_LEFT) { newW *= percentage; }
        else if (type == FillType::TOP_TO_BOTTOM) {
            y -= h; // change origin point to BOTTOM LEFT of the bar
            newH *= -percentage; 
        }

        // Bar
        GraphicsManager::DrawWorldUI(tex, GraphicsManager::CreateTransformMatrix(x, y, 0, newW, newH), color);
	}

    Panel::Panel(AEVec2 _size, Color _color, Utils::RectMode _rectMode)
        : color{ _color }, rectMode{ _rectMode } {
        transform.size = _size;

        // Set position based on screen width and height
        float x = 0, y = 0, width = transform.size.x, height = transform.size.y;
        float halfScreenW = Config::SCREEN_WIDTH * 0.5f, halfScreenH = Config::SCREEN_HEIGHT * 0.5f;
        float halfW = width * 0.5f, halfH = height * 0.5f;

        // Set position based on rect mode
        switch (rectMode) {
        case Utils::RectMode::RECT_TOP_LEFT:
            x = -halfScreenW;
            y = halfScreenH;
            break;
        case Utils::RectMode::RECT_TOP_CENTER:
            x = -halfW;
            y = halfScreenH;
            break;
        case Utils::RectMode::RECT_TOP_RIGHT:
            x = halfScreenW - width;
            y = halfScreenH;
            break;
        case Utils::RectMode::RECT_CENTER_LEFT:
            x = -halfScreenW;
            y = halfH;
            break;
        case Utils::RectMode::RECT_CENTER:
            x = -halfW;
            y = halfH;
            break;
        case Utils::RectMode::RECT_CENTER_RIGHT:
            x = halfScreenW - width;
            y = halfH;
            break;
        case Utils::RectMode::RECT_BOTTOM_LEFT:
            x = -halfScreenW;
            y = -halfScreenH + height;
            break;
        case Utils::RectMode::RECT_BOTTOM_CENTER:
            x = -halfW;
            y = -halfScreenH + height;
            break;
        case Utils::RectMode::RECT_BOTTOM_RIGHT:
            x = halfScreenW - width;
            y = -halfScreenH + height;
            break;
        }

        transform.pos = { x,y };
    }

    AEVec2 Panel::Pos() const { return transform.pos; }

    void Panel::SetChildPanelPos(AEVec2 const& parentPos, AEVec2 const& offset) {
        transform.pos = { parentPos.x + offset.x, parentPos.y + offset.y };
    }

    void Panel::Draw() {
        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, 0, transform.size.x, transform.size.y);
        GraphicsManager::DrawScreenOverlay(AssetsManager::squareTexture, mat, color);
    }
}