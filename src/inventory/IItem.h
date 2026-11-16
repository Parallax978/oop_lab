#pragma once

#include <memory>
#include <string>
#include <string_view>

class Robot;

class IItem {
public:
    virtual ~IItem() = default;
    virtual std::string_view getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool canUse(const Robot& owner) const = 0;
    virtual bool use(Robot& owner) = 0;
    virtual std::unique_ptr<IItem> clone() const = 0;
};
