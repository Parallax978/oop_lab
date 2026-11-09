#pragma once

#include <memory>
#include <string>
#include <string_view>

class Robot;

class IStatus {
public:
    virtual ~IStatus() = 0;

    virtual std::string_view getName() const = 0;
    virtual std::string getDescription() const { return std::string(getName()); }

    virtual bool canMove() const = 0;

    virtual bool onAbilityAttempt() = 0;

    virtual int processDamage(int damage) = 0;

    virtual void onTurnEnd(Robot& owner) = 0;
    virtual bool isExpired() const = 0;

    virtual bool merge(const IStatus& other) = 0;

    virtual std::unique_ptr<IStatus> clone() const = 0;
};

inline IStatus::~IStatus() = default;
