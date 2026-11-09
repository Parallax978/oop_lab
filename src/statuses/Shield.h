#pragma once

#include "IStatus.h"

class Shield : public IStatus {
public:
    explicit Shield(int damageCapacity = 30);

    std::string_view getName() const override;
    std::string getDescription() const override;
    bool canMove() const override;
    bool onAbilityAttempt() override;
    int processDamage(int damage) override;
    void onTurnEnd(Robot& owner) override;
    bool isExpired() const override;
    bool merge(const IStatus& other) override;
    std::unique_ptr<IStatus> clone() const override;

private:
    int damageCapacity_;
};
