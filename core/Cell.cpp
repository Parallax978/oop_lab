#include "../Cell.h"
#include <algorithm>

Cell::Cell(bool isWalkable, int moveCost)
    : isWalkable_(isWalkable),
      moveCost_(std::max(1, moveCost)),
      isDiscovered_(false) {}

bool Cell::isWalkable() const {
    return isWalkable_;
}

void Cell::setWalkable(bool walkable) {
    isWalkable_ = walkable;
}

int Cell::getMoveCost() const {
    return moveCost_;
}

void Cell::setMoveCost(int cost) {
    // Сохранение инварианта: стоимость шага не может быть <= 0
    moveCost_ = std::max(1, cost);
}

bool Cell::isDiscovered() const {
    return isDiscovered_;
}

void Cell::setDiscovered(bool discovered) {
    isDiscovered_ = discovered;
}