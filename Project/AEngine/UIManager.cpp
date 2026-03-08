#include "UIManager.hpp"

#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"

namespace UIManager {
    Bar::Bar(AEVec2 _pos, AEVec2 _size, Color _color, FillType _type, AEGfxTexture* _texture)
        : transform{ _pos, _size }, color{ _color }, type{ _type }, texture{_texture} {}

	void Bar::Draw(float currValue, float maxValue, bool isScreenOverlay) {
        AEGfxTexture* tex = AssetsManager::squareTexture;

        float x = transform.pos.x, y = transform.pos.y;
        float w = transform.size.x, h = transform.size.y;

        //float halfWidth = transform.size.x / 2.0f;
        //x -= halfWidth;

        // Lighter background bar
        Color bgColor = { color.Red(), color.Green(), color.Blue() };
        bgColor.EditColorPercentage(0.8f);

        if (isScreenOverlay) {
            GraphicsManager::DrawScreenOverlay(tex, GraphicsManager::CreateTransformMatrix(x, y, 0, w, h), bgColor);
        }
        else {
            GraphicsManager::DrawWorldUI(tex, GraphicsManager::CreateTransformMatrix(x, y, 0, w, h), bgColor);
        }

        float percentage = (currValue / maxValue);
        float newW = w, newH = h;

        switch (type) {
        case FillType::RIGHT_TO_LEFT:
            x += w * (1.0f - percentage); // shift origin so right edge stays fixed
            newW *= percentage;
            break;
        case FillType::LEFT_TO_RIGHT:
            newW *= percentage;
            break;
        case FillType::TOP_TO_BOTTOM:
            y -= h; // change origin point to BOTTOM LEFT of the bar
            newH *= -percentage;
            break;
        case FillType::BOTTOM_TO_TOP:
            newH *= percentage;
            break;
        }

        // Bar
        if (isScreenOverlay) {
            GraphicsManager::DrawScreenOverlay(tex, GraphicsManager::CreateTransformMatrix(x, y, 0, newW, newH), color);
        }
        else {
            GraphicsManager::DrawWorldUI(tex, GraphicsManager::CreateTransformMatrix(x, y, 0, newW, newH), color);
        }
	}

    Panel::Panel(AEVec2 _size, Color _color, Utils::AnchorMode _anchorMode, AEGfxTexture* _texture)
        : color{ _color }, anchorMode{ _anchorMode }, texture{_texture} {
        transform.size = _size;
        Utils::SetAnchoredPosition(transform, anchorMode);
    }

    AEVec2 Panel::Pos() const { return transform.pos; }

    AEVec2 Panel::Size() const { return transform.size; }

    void Panel::SetChildPanelPos(AEVec2 const& parentPos, AEVec2 const& offset) {
        transform.pos = { parentPos.x + offset.x, parentPos.y + offset.y };
    }

    void Panel::Draw() {
        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, 0, transform.size.x, transform.size.y);
        GraphicsManager::DrawScreenOverlay(AssetsManager::squareTexture, mat, color);
    }
}