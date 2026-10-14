#include "RobotFactory.h"
#include <algorithm>

RobotFactory::RobotFactory(const Point& topLeft, int spawnInterval)
    : topLeft_(topLeft),
      spawnInterval_(std::max(1, spawnInterval)),
      turnsUntilSpawn_(std::max(1, spawnInterval)) {}


bool RobotFactory::occupies(const Point& pt) const {
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