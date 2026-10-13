#include "RobotFactory.h"
#include <algorithm>

RobotFactory::RobotFactory(Point topLeft, int spawnInterval)
    : topLeft_(topLeft),
      spawnInterval_(std::max(1, spawnInterval)),
      turnsUntilSpawn_(std::max(1, spawnInterval)) {}

Point RobotFactory::getTopLeft() const {
    return topLeft_;
}

int RobotFactory::getSpawnInterval() const {
    return spawnInterval_;
}

int RobotFactory::getTurnsUntilSpawn() const {
    return turnsUntilSpawn_;
}

bool RobotFactory::occupies(Point pt) const {
    return (pt.x >= topLeft_.x && pt.x <= topLeft_.x + 1) &&
           (pt.y >= topLeft_.y && pt.y <= topLeft_.y + 1);
}

bool RobotFactory::tick() {
    turnsUntilSpawn_--;
    if (turnsUntilSpawn_ <= 0) {
        turnsUntilSpawn_ = spawnInterval_;
        return true;
    }
    return false;
}

void RobotFactory::resetTimer() {
    turnsUntilSpawn_ = spawnInterval_;
}