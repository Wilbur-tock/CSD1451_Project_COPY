#include "pch.hpp"
#include "Color.hpp"

namespace UIManager {
	enum FillType {
		RIGHT_TO_LEFT,
		TOP_TO_BOTTOM,
		//LEFT_TO_RIGHT,
		//BOTTOM_TO_TOP,
	};

	class Bar {
	public:
		AEVec2 pos;
		AEVec2 size;
		Color barColor;
		FillType type;

		// Filltype RTL - normal health bar
		Bar(AEVec2 _pos, AEVec2 _size, Color _barColor, FillType _type = RIGHT_TO_LEFT);
	};

	void DrawBar(Bar bar, float currValue, float maxValue);
}