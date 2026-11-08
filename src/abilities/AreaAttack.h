#pragma once

#include "IAbility.h"

class AreaAttack : public IAbility {
public:
    static constexpr int EnergyCost = 20;
    static constexpr int Damage = 20;

    std::string_view getName() const override;
    int getEnergyCost() const override;
    bool requiresTarget() const override;
    int getRange() const override;
    bool use(Robot& user, Field& field, const Point& target) override;
    void upgrade() override;

private:
    int radius_{1};
};
