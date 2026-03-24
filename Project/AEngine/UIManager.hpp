#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include "pch.hpp"

#include "Utils.hpp"
#include "Color.hpp"
#include "BaseTypes.hpp"

#include "TextObject.hpp"
#include "Button.hpp"

namespace UIManager {
	enum class FillType {
		LEFT_TO_RIGHT,
		RIGHT_TO_LEFT,
		TOP_TO_BOTTOM,
		BOTTOM_TO_TOP,
	};

	class Bar {
	public:
		Bar() = default;
		// Filltype RTL - normal health bar
		Bar(AEVec2 _pos, AEVec2 _size, Color _fillColor, FillType _type = FillType::LEFT_TO_RIGHT, AEGfxTexture* _texture = nullptr);
		Bar(AEVec2 _pos, AEVec2 _size, Color _fillColor, Color _bgColor, FillType _type = FillType::LEFT_TO_RIGHT, AEGfxTexture* _texture = nullptr);

		Transform GetTransform() const { return transform; }
		void UpdatePos(AEVec2 const& newPos) {
			transform.pos = newPos;
		}

		// Drawing bar on a screenoverlay or not
		void Draw(float currValue, float maxValue, bool isScreenOverlay = true);
	private:
		Transform transform{ {0,0},{0,0} };
		Color fillColor{ 1,1,1 };
		Color bgColor{ 1,1,1 };
		AEGfxTexture* texture = nullptr;
		FillType type = FillType::LEFT_TO_RIGHT;
	};

	class Panel {
	public:
		Panel() = default;
		// Draw a panel based on the rect mode with the the WORLD COORDINATES (0,0) as origin
		Panel(AEVec2 _size, Color _color = { 1,1,1 }, Utils::AnchorMode _anchorMode = Utils::AnchorMode::CENTER, AEGfxTexture* _texture = nullptr);

		Transform GetTransform() const { return transform; }
		void AddOffsetPosition(AEVec2 const& _pos) { transform.pos.x += _pos.x , transform.pos.y += _pos.y; }

		void SetTexture(AEGfxTexture* _texture) { texture = _texture; }
		void SetColor(Color newColor) { color = newColor; }

		// Assign a parent panel/object to this panel object and it will set its position 
		// based on the parent object and its rectMode
		void SetChildPanelPos(AEVec2 const& parentPos, AEVec2 const& offset);
		void Draw();
	private:
		Transform transform{ {0,0},{0,0} };
		Color color{ 1,1,1 };
		AEGfxTexture* texture = nullptr;
		Utils::AnchorMode anchorMode = Utils::AnchorMode::CENTER;
	};

	class PopupPanel {
	public:
		Panel panel;
		TextObject textObj;
		Button closeButton;

		PopupPanel() = default;
		PopupPanel(bool _hasTimer, Panel _panel, TextObject _textObj, Button _closeButton = Button{{0,0}, {50,50}, Color{0,0,0}, nullptr});

		bool IsHovered() const { return isHovered; }
		bool IsActive() const { return isActive; }
		// input offset position of the panel anchor position
		void AddOffsetPosition(AEVec2 const& offsetPos);

		void Popup(std::string msg, float lifespanTimer = 0.0f);
		void Update();
		void Draw();
	private:
		bool isHovered = false;
		bool isActive = false;
		bool hasTimer = false; // if true popup panel will be deactived once the timer run out
		float timer = 5.0f;
	};

	class IconWithText {
	public:
		void Initialize(Transform const& bgPanelTransform, Transform const& iconTransform, Utils::AnchorMode const& textAnchorMode = Utils::AnchorMode::BOTTOM_RIGHT);
		void OffsetPosition(float xOffset, float yOffset, float gapsBetweenBorder = 2.5f);
		void Draw(Color const& bgColor, AEGfxTexture* iconTexture, std::string textContent);
	private:
		Panel bgPanel, icon;
		TextObject textObj;
	};
}

#endif // !UIMANAGER_HPP

