#pragma once

#include "IItem.h"

class AttackBooster : public IItem {
public:
    static constexpr int AttackBonus = 10;
    static constexpr int Duration = 3;
    std::string_view getName() const override;
    std::string getDescription() const override;
    bool canUse(const Robot& owner) const override;
    bool use(Robot& owner) override;
    std::unique_ptr<IItem> clone() const override;
};
