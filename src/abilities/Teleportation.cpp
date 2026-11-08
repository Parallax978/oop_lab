#include "Teleportation.h"

#include <algorithm>
#include "Field.h"
#include "Robot.h"

std::string_view Teleportation::getName() const {
    return "Teleportation";
}

int Teleportation::getEnergyCost() const {
    return energyCost_;
}

bool Teleportation::requiresTarget() const {
    return true;
}

int Teleportation::getRange() const {
    return Range;
}

bool Teleportation::use(Robot& user, Field& field, const Point& target) {
    if (!user.isAlive()) return false;

    const auto position = field.getRobotPosition(&user);
    if (!position || position->distance(target) > getRange()) return false;
    if (user.getEnergy() < getEnergyCost()) return false;
    if (!field.teleportRobot(&user, target)) return false;

    user.consumeEnergy(getEnergyCost());
    return true;
}

void Teleportation::upgrade() {
    energyCost_ = std::max(MinEnergyCost, energyCost_ - EnergyCostUpgrade);
}
