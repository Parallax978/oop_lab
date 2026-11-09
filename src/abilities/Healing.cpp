#include "Healing.h"

#include "Field.h"
#include "Robot.h"
#include "Shield.h"

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

    const int missingHp = user.getMaxHp() - user.getHp();
    const int overheal = healing_ - missingHp;
    user.heal(healing_);
    if (overheal > 0) {
        user.addStatus(std::make_unique<Shield>(overheal));
    }
    return true;
}

void Healing::upgrade() {
    healing_ += HealingUpgrade;
}
