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

        // Lighter background bar
        if (isScreenOverlay) {
            GraphicsManager::DrawScreenOverlay(tex, GraphicsManager::CreateTransformMatrix(x, y, w, h), bgColor);
        }
        else {
            GraphicsManager::DrawWorldUI(tex, GraphicsManager::CreateTransformMatrix(x, y, w, h), bgColor);
        }

        float percentage = (currValue / maxValue);
        float newW = w, newH = h;

        // Change how the bar moves
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

        // Draw based on ui type
        if (isScreenOverlay) {
            GraphicsManager::DrawScreenOverlay(tex, GraphicsManager::CreateTransformMatrix(x, y, newW, newH), fillColor);
        }
        else {
            GraphicsManager::DrawWorldUI(tex, GraphicsManager::CreateTransformMatrix(x, y, newW, newH), fillColor);
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
        AEMtx33 mat = GraphicsManager::CreateTransformMatrix(transform.pos.x, transform.pos.y, transform.size.x, transform.size.y);
        AEGfxTexture* _texture = AssetsManager::squareTexture;
        if (texture) {
            _texture = texture;
        }
        GraphicsManager::DrawScreenOverlay(_texture, mat, color);
    }

    PopupPanel::PopupPanel(bool _hasTimer, Panel _panel, TextObject _textObj, Button _closeButton)
        : panel{ _panel },
        textObj{ _textObj },
        closeButton{ _closeButton },
        hasTimer{ _hasTimer } {
        // Set button position to be top right corner to close panel
        closeButton.SetPosition(panel.GetTransform(), Utils::AnchorMode::TOP_RIGHT);

        closeButton.SetOnClick([this]() {
            isActive = false;
        });

        textObj.SetPosition(panel.GetTransform());
        //std::cout << panel.Pos().x << ' ' << panel.Pos().y << " Pos: " << textObj.Pos().x << ' ' << textObj.Pos().y << '\n';
    }

    void PopupPanel::AddOffsetPosition(AEVec2 const& offsetPos) {
        panel.AddOffsetPosition(offsetPos);
        closeButton.AddOffsetPosition(offsetPos);

        // Align text object to be center of the panel
        textObj.SetPosition(Utils::AnchorMode::CENTER, panel.GetTransform());
    }

    void PopupPanel::Popup(std::string msg, float lifespanTimer) {
        textObj.TextContent(msg);
        isActive = true;
        timer = lifespanTimer;
    }

    void PopupPanel::Update() {
        if (!isActive) return;

        if (hasTimer) {
            // Check timer is zero to close the popup panel
            timer -= static_cast<float>(AEFrameRateControllerGetFrameTime());

            if (timer <= 0.0f) {
                isActive = false;
            }
        }
        else {
            // Check for button input to close the popup panel
            isHovered = CollisionManager::IsPointInRect(panel.GetTransform().pos.x, panel.GetTransform().pos.y, panel.GetTransform().size.x, panel.GetTransform().size.y);

            if (isHovered) {
                closeButton.Update();
            }

            if (closeButton.IsClick()) {
                isActive = false;
            }
        }
    }

    void PopupPanel::Draw() {
        if (!isActive) return;
        panel.Draw();

        if (!hasTimer) {
            closeButton.Draw();
        }

        textObj.Draw();
    }

    void IconWithText::Initialize(Transform const& bgPanelTransform, Transform const& iconTransform, Utils::AnchorMode const& textAnchorMode) {
        bgPanel = UIManager::Panel{
            {bgPanelTransform.size}, PanelColor(), Utils::AnchorMode::TOP_CENTER
        };

        icon = UIManager::Panel{
            {iconTransform.size}, Color{1,1,1}, Utils::AnchorMode::TOP_CENTER
        };

        textObj = TextObject{ "5", 0.55f, Color{0.15f, 0.15f, 0.15f}, { {0,0}, {0,0} }, textAnchorMode };
        textObj.SetPosition(bgPanel.GetTransform());
    }

    void IconWithText::OffsetPosition(float xOffset, float yOffset, float gapsBetweenBorder) {
        bgPanel.AddOffsetPosition({ xOffset, yOffset });
        icon.AddOffsetPosition({ xOffset, yOffset - gapsBetweenBorder });
        textObj.SetPosition(bgPanel.GetTransform()); // set anchor position according to bg panel position
    }

    void IconWithText::Draw(Color const& bgColor, AEGfxTexture* iconTexture, std::string textContent) {
        bgPanel.SetColor(bgColor);
        bgPanel.Draw();
        // Update texture icon then draw
        icon.SetTexture(iconTexture);
        icon.Draw();
        // Update the number of turns remaining
        textObj.TextContent(textContent);
        textObj.Draw();
    }

    PanelText::PanelText(Panel _panel, TextObject _textObj)
        : bgPanel{ _panel }, textObj{_textObj} {
        textObj.SetPosition(bgPanel.GetTransform());
    }

    void PanelText::Update(std::string text) {
        textObj.TextContent(text);
    }

    void PanelText::Draw() {
        bgPanel.Draw();
        textObj.Draw();
    }
}