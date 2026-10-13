#pragma once

struct Point {
    int x{0};
    int y{0};

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }

    int distance(const Point& other) const;
};