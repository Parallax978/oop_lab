#include "EnergyBattery.h"
#include "Robot.h"

std::string_view EnergyBattery::getName() const { return "Energy battery"; }
std::string EnergyBattery::getDescription() const {
    return "Restores " + std::to_string(RestoreAmount) + " energy";
}
bool EnergyBattery::canUse(const Robot& owner) const {
    return owner.isPlayer() && owner.isAlive() && owner.getEnergy() < owner.getMaxEnergy();
}
bool EnergyBattery::use(Robot& owner) {
    if (!canUse(owner)) return false;
    owner.restoreEnergy(RestoreAmount);
    return true;
}
std::unique_ptr<IItem> EnergyBattery::clone() const {
    return std::make_unique<EnergyBattery>(*this);
}
