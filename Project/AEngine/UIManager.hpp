#include "pch.hpp"

//#include "TextObject.hpp"
//#include "Button.hpp"

#include "Utils.hpp"
#include "Color.hpp"
#include "BaseTypes.hpp"

namespace UIManager {
	enum class FillType {
		LEFT_TO_RIGHT,
		RIGHT_TO_LEFT,
		TOP_TO_BOTTOM,
		BOTTOM_TO_TOP,
	};

	class Bar {
	public:
		// Filltype RTL - normal health bar
		Bar(AEVec2 _pos, AEVec2 _size, Color _color, FillType _type = FillType::LEFT_TO_RIGHT, AEGfxTexture* _texture = nullptr);

		// Drawing bar on a screenoverlay or not
		void Draw(float currValue, float maxValue, bool isScreenOverlay = true);
	private:
		Transform transform;
		Color color;
		AEGfxTexture* texture;
		FillType type;
	};

	class Panel {
	public:
		// Draw a panel based on the rect mode with the the WORLD COORDINATES (0,0) as origin
		Panel(AEVec2 _size, Color _color = { 1,1,1 }, Utils::AnchorMode _anchorMode = Utils::AnchorMode::CENTER, AEGfxTexture* _texture = nullptr);

		AEVec2 Pos() const { return transform.pos; }
		void AddOffset(AEVec2 const& _pos) { transform.pos.x += _pos.x , transform.pos.y += _pos.y; }
		AEVec2 Size() const { return transform.size; }

		// Assign a parent panel/object to this panel object and it will set its position 
		// based on the parent object and its rectMode
		void SetChildPanelPos(AEVec2 const& parentPos, AEVec2 const& offset);
		void Draw();
	private:
		Transform transform;
		Color color;
		AEGfxTexture* texture;
		Utils::AnchorMode anchorMode;
	};
}