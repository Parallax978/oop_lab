#include "Cell.h"
#include <algorithm>
#include <utility>
#include "Robot.h"

Cell::Cell(bool isWalkable, int moveCost)
    : isWalkable_(isWalkable),
      moveCost_(std::max(1, moveCost)) {}

Cell::Cell(const Cell& other)
    : isWalkable_(other.isWalkable_), moveCost_(other.moveCost_),
      isDiscovered_(other.isDiscovered_) {
    for (const auto& status : other.statuses_) {
        statuses_.push_back(status->clone());
    }
}

Cell& Cell::operator=(const Cell& other) {
    if (this != &other) {
        Cell copy(other);
        *this = std::move(copy);
    }
    return *this;
}

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

bool Cell::addStatus(std::unique_ptr<IStatus> status) {
    if (!status || status->isExpired()) return false;
    for (const auto& current : statuses_) {
        if (current->merge(*status)) return true;
    }
    statuses_.push_back(std::move(status));
    return true;
}

std::size_t Cell::getStatusCount() const {
    return statuses_.size();
}

const IStatus* Cell::getStatus(std::size_t index) const {
    if (index >= statuses_.size()) return nullptr;
    return statuses_[index].get();
}

void Cell::applyStatuses(Robot& robot) const {
    for (const auto& status : statuses_) {
        robot.addStatus(status->clone());
    }
}
