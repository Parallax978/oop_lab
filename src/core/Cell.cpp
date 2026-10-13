#include "Cell.h"
#include <algorithm>

Cell::Cell(bool isWalkable, int moveCost)
    : isWalkable_(isWalkable),
      moveCost_(std::max(1, moveCost)) {}

bool Cell::isWalkable() const {
    return isWalkable_;
}

int Cell::getMoveCost() const {
    return moveCost_;
}

bool Cell::isDiscovered() const {
    return isDiscovered_;
}


void Cell::setWalkable(bool walkable) {
    isWalkable_ = walkable;
}

void Cell::setMoveCost(int cost) {
    moveCost_ = std::max(1, cost);
}

void Cell::setDiscovered(bool discovered) {
    isDiscovered_ = discovered;
}