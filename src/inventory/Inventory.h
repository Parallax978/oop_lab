#pragma once

#include <vector>
#include "IInventory.h"

class Inventory : public IInventory {
public:
    static constexpr std::size_t DefaultCapacity = 6;
    explicit Inventory(std::size_t capacity = DefaultCapacity);

    std::size_t getItemCount() const override;
    std::size_t getCapacity() const override;
    const IItem* getItem(std::size_t index) const override;
    bool addItem(std::unique_ptr<IItem> item) override;
    bool useItem(std::size_t index, Robot& owner) override;

private:
    std::size_t capacity_;
    std::vector<std::unique_ptr<IItem>> items_;
};
