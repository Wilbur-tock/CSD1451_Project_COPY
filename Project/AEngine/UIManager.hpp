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
		AEVec2 Pos() const { return transform.pos; }
		void AddOffset(AEVec2 const& _pos) { transform.pos.x += _pos.x , transform.pos.y += _pos.y; }
		AEVec2 Size() const { return transform.size; }

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
		Button button;
		TextObject textObj;

		PopupPanel(std::string textContent, AEVec2 _size, Color _color = { 1,1,1 }, Utils::AnchorMode panelAnchorMode = Utils::AnchorMode::CENTER, AEGfxTexture* _texture = nullptr,
			AEGfxTexture* btnTexture = nullptr, Utils::FuncPtr onClick = nullptr);

		bool IsHovered() const { return isHovered; }
		bool IsActive() const { return isActive; }

		void Popup(std::string msg);
		void Draw();
		void Update();
	private:
		bool isHovered = false;
		bool isActive = false;
	};
}

#endif // !UIMANAGER_HPP

