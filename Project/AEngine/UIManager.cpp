#include "UIManager.hpp"

#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"
#include "CollisionManager.hpp"

namespace UIManager {
    Bar::Bar(AEVec2 _pos, AEVec2 _size, Color _color, FillType _type, AEGfxTexture* _texture)
        : transform{ _pos, _size }, fillColor{ _color }, bgColor{ _color }, type{ _type }, texture{_texture} {
        bgColor.EditColorPercentage(0.8f);
    }

    Bar::Bar(AEVec2 _pos, AEVec2 _size, Color _color, Color _bgColor, FillType _type, AEGfxTexture* _texture)
        : transform{ _pos, _size }, fillColor{ _color }, bgColor{_bgColor}, type{_type}, texture{_texture} {
    }

	void Bar::Draw(float currValue, float maxValue, bool isScreenOverlay) {
        AEGfxTexture* tex = AssetsManager::squareTexture;

        float x = transform.pos.x, y = transform.pos.y;
        float w = transform.size.x, h = transform.size.y;

        //float halfWidth = transform.size.x / 2.0f;
        //x -= halfWidth;

        // Lighter background bar
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
            GraphicsManager::DrawScreenOverlay(tex, GraphicsManager::CreateTransformMatrix(x, y, 0, newW, newH), fillColor);
        }
        else {
            GraphicsManager::DrawWorldUI(tex, GraphicsManager::CreateTransformMatrix(x, y, 0, newW, newH), fillColor);
        }
	}

    Panel::Panel(AEVec2 _size, Color _color, Utils::AnchorMode _anchorMode, AEGfxTexture* _texture)
        : color{ _color }, anchorMode{ _anchorMode }, texture{_texture} {
        transform.size = _size;
        Utils::SetAnchoredPosition(transform, anchorMode);
    }

    void Panel::SetChildPanelPos(AEVec2 const& parentPos, AEVec2 const& offset) {
        transform.pos = { parentPos.x + offset.x, parentPos.y + offset.y };
    }

    void Panel::Draw() {
        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, 0, transform.size.x, transform.size.y);
        GraphicsManager::DrawScreenOverlay(AssetsManager::squareTexture, mat, color);
    }

    PopupPanel::PopupPanel(std::string textContent, AEVec2 _size, Color _color, Utils::AnchorMode panelAnchorMode, AEGfxTexture* panelTexture, AEGfxTexture* btnTexture, Utils::FuncPtr onClick) 
        : panel{ _size, _color, panelAnchorMode, panelTexture }, 
        button{ {0,0}, {50,50}, Color{1,1,1}, btnTexture, onClick },
        textObj{ textContent, 0.5f, Color{1,0,0}, {0,0}, Utils::AnchorMode::CENTER } {
        // Set button position to be top right corner to close panel
        button.SetPosition({panel.Pos(), panel.Size()}, Utils::AnchorMode::TOP_RIGHT);

        button.SetOnClick([this]() {
            isActive = false;
        });

        textObj.SetPosition(Utils::AnchorMode::CENTER, panel.GetTransform());
        //std::cout << panel.Pos().x << ' ' << panel.Pos().y << " Pos: " << textObj.Pos().x << ' ' << textObj.Pos().y << '\n';
    }

    void PopupPanel::Popup(std::string msg) {
        textObj.TextContent(msg);
        isActive = true;
    }

    void PopupPanel::Draw() {
        if (!isActive) return;
        panel.Draw();
        button.Draw();
        textObj.Draw();
    }

    void PopupPanel::Update() {
        if (!isActive) return;

        isHovered = CollisionManager::IsPointInRect(panel.Pos().x, panel.Pos().y, panel.Size().x, panel.Size().y);

        if (isHovered) {
            button.Update();
        }

        if (button.IsClick()) {
            isActive = false;
        }
    }
}