#include "TileIndex.hpp"

#include "Utils.hpp"

bool TileIndex::operator==(TileIndex const& rhs) const {
    return x == rhs.x && y == rhs.y;
}

void TileIndex::operator=(TileIndex const& rhs) {
	x = rhs.x;
	y = rhs.y;
}

bool TileIndex::IsOutOfBounds() const {
	return Utils::IsGridIndexOutOfBounds(x, y);
}

std::ostream& operator<<(std::ostream& os, TileIndex rhs) {
	os << "x: " << rhs.x << " y: " << rhs.y;
	return os;
}