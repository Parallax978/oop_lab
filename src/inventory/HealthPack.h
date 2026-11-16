#pragma once

#include "IItem.h"

class HealthPack : public IItem {
public:
    static constexpr int RestoreAmount = 30;
    std::string_view getName() const override;
    std::string getDescription() const override;
    bool canUse(const Robot& owner) const override;
    bool use(Robot& owner) override;
    std::unique_ptr<IItem> clone() const override;
};
