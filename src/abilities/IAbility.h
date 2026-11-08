#pragma once

#include <string_view>

class Robot;
class Field;
struct Point;

class IAbility {
public:
    virtual ~IAbility() = 0;

    virtual std::string_view getName() const = 0;
    virtual int getEnergyCost() const = 0;
    virtual bool requiresTarget() const = 0;
    virtual int getRange() const = 0;

    virtual bool use(Robot& user, Field& field, const Point& target) = 0;

    virtual void upgrade() = 0;
};

inline IAbility::~IAbility() = default;
