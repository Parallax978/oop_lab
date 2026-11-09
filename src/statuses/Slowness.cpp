#include "Slowness.h"

#include <algorithm>

Slowness::Slowness(int turns)
    : turnsLeft_(std::max(0, turns)) {}

std::string_view Slowness::getName() const {
    return "Slowness";
}

std::string Slowness::getDescription() const {
    return std::string(getName()) + " (turns left: " + std::to_string(turnsLeft_) + ")";
}

bool Slowness::canMove() const {
    return isExpired();
}

bool Slowness::onAbilityAttempt() {
    return true;
}

int Slowness::processDamage(int damage) {
    return damage;
}

void Slowness::onTurnEnd(Robot&) {
    if (turnsLeft_ > 0) --turnsLeft_;
}

bool Slowness::isExpired() const {
    return turnsLeft_ == 0;
}

bool Slowness::merge(const IStatus& other) {
    const auto* slowness = dynamic_cast<const Slowness*>(&other);
    if (!slowness) return false;

    turnsLeft_ += slowness->turnsLeft_;
    return true;
}

std::unique_ptr<IStatus> Slowness::clone() const {
    return std::make_unique<Slowness>(*this);
}
