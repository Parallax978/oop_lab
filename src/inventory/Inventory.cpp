#include "Inventory.h"

#include <utility>

Inventory::Inventory(std::size_t capacity) : capacity_(capacity) {}

std::size_t Inventory::getItemCount() const { return items_.size(); }
std::size_t Inventory::getCapacity() const { return capacity_; }

const IItem* Inventory::getItem(std::size_t index) const {
    return index < items_.size() ? items_[index].get() : nullptr;
}

bool Inventory::addItem(std::unique_ptr<IItem> item) {
    if (!item || items_.size() >= capacity_) return false;
    items_.push_back(std::move(item));
    return true;
}

bool Inventory::useItem(std::size_t index, Robot& owner) {
    if (index >= items_.size() || !items_[index]->canUse(owner)) return false;
    if (!items_[index]->use(owner)) return false;
    items_.erase(items_.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}
