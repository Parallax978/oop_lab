#pragma once
#include <cstddef>
#include <memory>
#include <vector>
#include "IStatus.h"

class Robot;

class Cell {
public:
    Cell(bool isWalkable = true, int moveCost = 1);
    Cell(const Cell& other);
    Cell& operator=(const Cell& other);
    Cell(Cell&&) noexcept = default;
    Cell& operator=(Cell&&) noexcept = default;

    bool isWalkable() const;
    int getMoveCost() const;
    bool isDiscovered() const;

    void setWalkable(bool walkable);
    void setMoveCost(int cost);
    void setDiscovered(bool discovered);

    bool addStatus(std::unique_ptr<IStatus> status);
    std::size_t getStatusCount() const;
    const IStatus* getStatus(std::size_t index) const;
    void applyStatuses(Robot& robot) const;

private:
    bool isWalkable_{true};
    int moveCost_{1};
    bool isDiscovered_{false};
    std::vector<std::unique_ptr<IStatus>> statuses_;
};
