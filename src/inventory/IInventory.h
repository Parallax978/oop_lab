#pragma once

#include <cstddef>

class IInventory {
public:
    virtual ~IInventory() = 0;

    virtual std::size_t getItemCount() const = 0;
    virtual std::size_t getCapacity() const = 0;
};

inline IInventory::~IInventory() = default;
