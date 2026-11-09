#include "Shield.h"

#include <algorithm>

Shield::Shield(int damageCapacity) : damageCapacity_(std::max(0, damageCapacity)) {}

std::string_view Shield::getName() const {
    return "Shield";
}

std::string Shield::getDescription() const {
    return std::string(getName()) + " (damage capacity: " +
           std::to_string(damageCapacity_) + ")";
}

bool Shield::canMove() const {
    return true;
}

bool Shield::onAbilityAttempt() {
    return true;
}

int Shield::processDamage(int damage) {
    if (damage <= 0) return 0;
    const int blocked = std::min(damage, damageCapacity_);
    damageCapacity_ -= blocked;
    return damage - blocked;
}

void Shield::onTurnEnd(Robot&) {}

bool Shield::isExpired() const {
    return damageCapacity_ == 0;
}

bool Shield::merge(const IStatus& other) {
    const auto* shield = dynamic_cast<const Shield*>(&other);
    if (!shield) return false;
    damageCapacity_ += shield->damageCapacity_;
    return true;
}

std::unique_ptr<IStatus> Shield::clone() const {
    return std::make_unique<Shield>(*this);
}
