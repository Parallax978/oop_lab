#pragma once

class Cell {
public:
    Cell(bool isWalkable = true, int moveCost = 1);

    bool isWalkable() const;
    void setWalkable(bool walkable);

    int getMoveCost() const;
    void setMoveCost(int cost);

    bool isDiscovered() const;
    void setDiscovered(bool discovered);

private:
    bool isWalkable_{true};
    int moveCost_{1};          // Базовая стоимость шага: 1. Вязкие/сложные клетки: 2, 3 и т.д.
    bool isDiscovered_{false};  // Открыта ли клетка игроком в тумане войны
};