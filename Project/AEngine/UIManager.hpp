#include "pch.hpp"

#include "Utils.hpp"
#include "Color.hpp"
#include "BaseTypes.hpp"

namespace UIManager {
	enum class FillType {
		RIGHT_TO_LEFT,
		TOP_TO_BOTTOM,
		//LEFT_TO_RIGHT,
		//BOTTOM_TO_TOP,
	};

	class Bar {
	public:
		// Filltype RTL - normal health bar
		Bar(AEVec2 _pos, AEVec2 _size, Color _color, FillType _type = FillType::RIGHT_TO_LEFT);

		void Draw(float currValue, float maxValue);
	private:
		Transform transform;
		Color color;
		FillType type;
	};

	class Panel {
	public:
		// Draw a panel based on the rect mode with the the WORLD COORDINATES (0,0) as origin
		Panel(AEVec2 _size, Color _color = { 1,1,1 }, Utils::RectMode _rectMode = Utils::RectMode::RECT_CENTER);

		AEVec2 Pos() const;
		AEVec2 Size() const;

		void SetChildPanelPos(AEVec2 const& parentPos, AEVec2 const& offset);
		void Draw();
	private:
		Transform transform;
		Color color;
		Utils::RectMode rectMode;
	};
}