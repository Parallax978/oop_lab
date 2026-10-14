#pragma once
#include "Point.h"

class RobotFactory {
public:
    RobotFactory(const Point& topLeft, int spawnInterval);

    Point getTopLeft() const { return topLeft_; };
    int getSpawnInterval() const { return spawnInterval_; };
    int getTurnsUntilSpawn() const { return turnsUntilSpawn_; };


    bool occupies(const Point& pt) const;

    bool tick();

    void resetTimer();

private:
    Point topLeft_;
    int spawnInterval_;
    int turnsUntilSpawn_;
};