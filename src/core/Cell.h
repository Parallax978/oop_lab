#pragma once

class Cell {
public:
    Cell(bool isWalkable = true, int moveCost = 1);

    bool isWalkable() const;
    int getMoveCost() const;
    bool isDiscovered() const;

    void setWalkable(bool walkable);
    void setMoveCost(int cost);
    void setDiscovered(bool discovered);

private:
    bool isWalkable_{true};
    int moveCost_{1};
    bool isDiscovered_{false};
};