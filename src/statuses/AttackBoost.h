#pragma once

#include "IStatus.h"

class AttackBoost : public IStatus {
public:
    explicit AttackBoost(int bonus = 10, int turns = 3);
    std::string_view getName() const override;
    std::string getDescription() const override;
    int modifyAttack(int attack) const override;
    bool canMove() const override;
    bool onAbilityAttempt() override;
    int processDamage(int damage) override;
    void onTurnEnd(Robot& owner) override;
    bool isExpired() const override;
    bool merge(const IStatus& other) override;
    std::unique_ptr<IStatus> clone() const override;

private:
    int bonus_;
    int turnsLeft_;
};
