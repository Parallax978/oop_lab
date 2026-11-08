#pragma once

#include "IAbility.h"

class Healing : public IAbility {
public:
    static constexpr int EnergyCost = 20;
    static constexpr int InitialHealing = 30;
    static constexpr int HealingUpgrade = 10;

    std::string_view getName() const override;
    int getEnergyCost() const override;
    bool requiresTarget() const override;
    int getRange() const override;
    bool use(Robot& user, Field& field, const Point& target) override;
    void upgrade() override;

private:
    int healing_{InitialHealing};
};
