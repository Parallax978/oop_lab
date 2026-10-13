#pragma once
#include "Point.h"

class RobotFactory {
public:
    RobotFactory(Point topLeft, int spawnInterval);

    Point getTopLeft() const;
    int getSpawnInterval() const;
    int getTurnsUntilSpawn() const;

    // Занимает ли завод данную точку (проверка площади 2х2)
    bool occupies(Point pt) const;

    // Уменьшает счетчик ходов. Возвращает true, если на этом ходу нужно создать робота
    bool tick();

    void resetTimer();

private:
    Point topLeft_;
    int spawnInterval_;
    int turnsUntilSpawn_;
};