#include "HealthPack.h"
#include "Robot.h"

std::string_view HealthPack::getName() const { return "Health pack"; }
std::string HealthPack::getDescription() const {
    return "Restores " + std::to_string(RestoreAmount) + " HP";
}
bool HealthPack::canUse(const Robot& owner) const {
    return owner.isPlayer() && owner.isAlive() && owner.getHp() < owner.getMaxHp();
}
bool HealthPack::use(Robot& owner) {
    if (!canUse(owner)) return false;
    owner.heal(RestoreAmount);
    return true;
}
std::unique_ptr<IItem> HealthPack::clone() const {
    return std::make_unique<HealthPack>(*this);
}
