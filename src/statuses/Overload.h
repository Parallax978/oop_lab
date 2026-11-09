#pragma once

#include "IStatus.h"

class Overload : public IStatus {
public:
    explicit Overload(int turns = 1);

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
    int turnsLeft_;
};
