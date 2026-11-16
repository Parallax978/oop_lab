#pragma once

#include <memory>
#include "Robot.h"

std::unique_ptr<Robot> createEnemyRobot(int attack, int maxEnergy);
