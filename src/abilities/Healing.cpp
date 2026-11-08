#include "Healing.h"

#include "Field.h"
#include "Robot.h"

std::string_view Healing::getName() const {
    return "Healing";
}

int Healing::getEnergyCost() const {
    return EnergyCost;
}

bool Healing::requiresTarget() const {
    return false;
}

int Healing::getRange() const {
    return 0;
}

bool Healing::use(Robot& user, Field& field, const Point&) {
    if (!user.isAlive() || !field.getRobotPosition(&user).has_value()) return false;
    if (!user.consumeEnergy(getEnergyCost())) return false;

    user.heal(healing_);
    return true;
}

void Healing::upgrade() {
    healing_ += HealingUpgrade;
}
