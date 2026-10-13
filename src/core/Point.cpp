#include "../point.h"
#include <algorithm>
#include <cmath>

int Point::distance(const Point& other) const {
    return std::max(std::abs(x - other.x), std::abs(y - other.y));
}
