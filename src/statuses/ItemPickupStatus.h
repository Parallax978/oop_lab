#pragma once

#include "IStatus.h"
#include "IItem.h"

class ItemPickupStatus : public IStatus {
public:
    explicit ItemPickupStatus(std::unique_ptr<IItem> item);
    std::string_view getName() const override;
    std::string getDescription() const override;
    bool onApply(Robot& owner) override;
    bool canMove() const override;
    bool onAbilityAttempt() override;
    int processDamage(int damage) override;
    void onTurnEnd(Robot& owner) override;
    bool isExpired() const override;
    bool merge(const IStatus& other) override;
    std::unique_ptr<IStatus> clone() const override;

private:
    std::unique_ptr<IItem> item_;
    bool collected_{false};
};
