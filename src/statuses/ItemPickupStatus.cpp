#include "ItemPickupStatus.h"

#include <utility>
#include "Robot.h"

ItemPickupStatus::ItemPickupStatus(std::unique_ptr<IItem> item) : item_(std::move(item)) {}
std::string_view ItemPickupStatus::getName() const { return "Item pickup"; }
std::string ItemPickupStatus::getDescription() const {
    return item_ ? "Pickup: " + std::string(item_->getName()) + " - " + item_->getDescription()
                 : "Empty item pickup";
}
bool ItemPickupStatus::onApply(Robot& owner) {
    if (isExpired() || !owner.isPlayer() || !owner.isAlive()) return false;
    auto* inventory = owner.getInventory();
    if (!inventory || !inventory->addItem(item_->clone())) return false;
    collected_ = true;
    return true;
}
bool ItemPickupStatus::canMove() const { return true; }
bool ItemPickupStatus::onAbilityAttempt() { return true; }
int ItemPickupStatus::processDamage(int damage) { return damage; }
void ItemPickupStatus::onTurnEnd(Robot&) {}
bool ItemPickupStatus::isExpired() const { return collected_ || !item_; }
bool ItemPickupStatus::merge(const IStatus&) { return false; }
std::unique_ptr<IStatus> ItemPickupStatus::clone() const {
    auto copy = std::make_unique<ItemPickupStatus>(item_ ? item_->clone() : nullptr);
    copy->collected_ = collected_;
    return copy;
}
