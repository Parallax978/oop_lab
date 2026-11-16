#include "AttackBoost.h"
#include <algorithm>

AttackBoost::AttackBoost(int bonus, int turns)
    : bonus_(std::max(0, bonus)), turnsLeft_(std::max(0, turns)) {}

std::string_view AttackBoost::getName() const { return "Attack boost"; }
std::string AttackBoost::getDescription() const {
    return "Attack +" + std::to_string(bonus_) + " (rounds left: " + std::to_string(turnsLeft_) + ")";
}
int AttackBoost::modifyAttack(int attack) const {
    return isExpired() ? attack : attack + bonus_;
}
bool AttackBoost::canMove() const { return true; }
bool AttackBoost::onAbilityAttempt() { return true; }
int AttackBoost::processDamage(int damage) { return damage; }
void AttackBoost::onTurnEnd(Robot&) {
    if (turnsLeft_ > 0) --turnsLeft_;
}
bool AttackBoost::isExpired() const { return turnsLeft_ == 0 || bonus_ == 0; }
bool AttackBoost::merge(const IStatus& other) {
    const auto* boost = dynamic_cast<const AttackBoost*>(&other);
    if (!boost || bonus_ != boost->bonus_) return false;
    turnsLeft_ += boost->turnsLeft_;
    return true;
}
std::unique_ptr<IStatus> AttackBoost::clone() const {
    return std::make_unique<AttackBoost>(*this);
}
