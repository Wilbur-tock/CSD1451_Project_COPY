#ifndef BASETYPES_HPP
#define BASETYPES_HPP

#include "AEEngine.h"

// Store data of the position and size of the object
struct Transform {
	AEVec2 pos, size;
};

// Gaps between x left and right, y top and bottom
struct BorderGaps {
	float xLeft;
	float xRight;
	float yTop;
	float yBottom;
};

#endif // !BASETYPES_HPP
