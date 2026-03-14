#ifndef TILEINDEX_HPP
#define TILEINDEX_HPP

#include "pch.hpp"

struct TileIndex {
    int x; // Column
    int y; // Row

    bool operator==(TileIndex const& rhs) const;
    void operator=(TileIndex const& rhs);
    bool operator<=(TileIndex const& rhs) const;
    bool operator>=(TileIndex const& rhs) const;

    bool IsOutOfBounds() const;
};

std::ostream& operator<<(std::ostream& os, TileIndex rhs);

#endif // !GRIDINDEX_HPP
