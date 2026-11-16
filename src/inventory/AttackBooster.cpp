#include "AttackBooster.h"
#include "AttackBoost.h"
#include "Robot.h"

std::string_view AttackBooster::getName() const { return "Attack booster"; }
std::string AttackBooster::getDescription() const {
    return "+" + std::to_string(AttackBonus) + " attack for " + std::to_string(Duration) + " rounds";
}
bool AttackBooster::canUse(const Robot& owner) const {
    return owner.isPlayer() && owner.isAlive();
}
bool AttackBooster::use(Robot& owner) {
    if (!canUse(owner)) return false;
    return owner.addStatus(std::make_unique<AttackBoost>(AttackBonus, Duration));
}
std::unique_ptr<IItem> AttackBooster::clone() const {
    return std::make_unique<AttackBooster>(*this);
}
