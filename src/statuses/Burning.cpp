#include "Burning.h"

#include <algorithm>
#include "Robot.h"

Burning::Burning(int turns, int damagePerTurn)
    : turnsLeft_(std::max(0, turns)), damagePerTurn_(std::max(0, damagePerTurn)) {}

std::string_view Burning::getName() const {
    return "Burning";
}

std::string Burning::getDescription() const {
    return std::string(getName()) + " (turns left: " + std::to_string(turnsLeft_) +
           ", damage per turn: " + std::to_string(damagePerTurn_) + ")";
}

bool Burning::canMove() const {
    return true;
}

bool Burning::onAbilityAttempt() {
    return true;
}

int Burning::processDamage(int damage) {
    return damage;
}

void Burning::onTurnEnd(Robot& owner) {
    if (isExpired()) return;
    owner.takeDamage(damagePerTurn_);
    --turnsLeft_;
}

bool Burning::isExpired() const {
    return turnsLeft_ == 0 || damagePerTurn_ == 0;
}

bool Burning::merge(const IStatus& other) {
    const auto* burning = dynamic_cast<const Burning*>(&other);
    if (!burning || damagePerTurn_ != burning->damagePerTurn_) return false;
    turnsLeft_ += burning->turnsLeft_;
    return true;
}

std::unique_ptr<IStatus> Burning::clone() const {
    return std::make_unique<Burning>(*this);
}
