#include "RangedAttack.h"

#include "Field.h"
#include "Robot.h"

std::string_view RangedAttack::getName() const {
    return "Ranged attack";
}

int RangedAttack::getEnergyCost() const {
    return EnergyCost;
}

int RangedAttack::getRange() const {
    return Range;
}

bool RangedAttack::requiresTarget() const {
    return true;
}

bool RangedAttack::use(Robot& user, Field& field, const Point& target) {
    if (!user.isAlive()) return false;

    const auto position = field.getRobotPosition(&user);
    if (!position.has_value() || !field.isInside(target)) return false;
    if (position->distance(target) > Range) return false;

    Robot* victim = field.getRobotAt(target);
    if (!victim || victim == &user || !victim->isAlive()) return false;
    if (!user.consumeEnergy(getEnergyCost())) return false;

    victim->takeDamage(damage_);
    if (!victim->isAlive()) {
        user.addExp(50 * victim->getRank());
    }
    return true;
}

void RangedAttack::upgrade() {
    damage_ += DamageUpgrade;
}
