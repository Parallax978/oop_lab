#include "Overload.h"

#include <algorithm>

Overload::Overload(int turns) : turnsLeft_(std::max(0, turns)) {}

std::string_view Overload::getName() const {
    return "Overload";
}

std::string Overload::getDescription() const {
    return std::string(getName()) + " (turns left: " + std::to_string(turnsLeft_) + ")";
}

bool Overload::canMove() const {
    return true;
}

bool Overload::onAbilityAttempt() {
    return isExpired();
}

int Overload::processDamage(int damage) {
    return damage;
}

void Overload::onTurnEnd(Robot&) {
    if (turnsLeft_ > 0) --turnsLeft_;
}

bool Overload::isExpired() const {
    return turnsLeft_ == 0;
}

bool Overload::merge(const IStatus& other) {
    const auto* overload = dynamic_cast<const Overload*>(&other);
    if (!overload) return false;
    turnsLeft_ += overload->turnsLeft_;
    return true;
}

std::unique_ptr<IStatus> Overload::clone() const {
    return std::make_unique<Overload>(*this);
}
