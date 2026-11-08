#pragma once

#include "IAbility.h"

class Teleportation : public IAbility {
public:
    static constexpr int Range = 5;
    static constexpr int InitialEnergyCost = 20;
    static constexpr int EnergyCostUpgrade = 5;
    static constexpr int MinEnergyCost = 5;

    std::string_view getName() const override;
    int getEnergyCost() const override;
    bool requiresTarget() const override;
    int getRange() const override;
    bool use(Robot& user, Field& field, const Point& target) override;
    void upgrade() override;

private:
    int energyCost_{InitialEnergyCost};
};
