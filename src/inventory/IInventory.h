#pragma once

#include <cstddef>
#include <memory>
#include "IItem.h"

class Robot;

class IInventory {
public:
    virtual ~IInventory() = 0;

    virtual std::size_t getItemCount() const = 0;
    virtual std::size_t getCapacity() const = 0;
    virtual const IItem* getItem(std::size_t index) const = 0;
    virtual bool addItem(std::unique_ptr<IItem> item) = 0;
    virtual bool useItem(std::size_t index, Robot& owner) = 0;
};

inline IInventory::~IInventory() = default;
