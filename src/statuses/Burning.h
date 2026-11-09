#pragma once

#include "IStatus.h"

class Burning : public IStatus {
public:
    explicit Burning(int turns = 3, int damagePerTurn = 10);

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
    int damagePerTurn_;
};
