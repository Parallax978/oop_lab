#include "AreaAttack.h"

#include "Field.h"
#include "Robot.h"
#include "Overload.h"

std::string_view AreaAttack::getName() const {
    return "Area attack";
}

int AreaAttack::getEnergyCost() const {
    return EnergyCost;
}

int AreaAttack::getRange() const {
    return radius_;
}

bool AreaAttack::requiresTarget() const {
    return false;
}

bool AreaAttack::use(Robot& user, Field& field, const Point&) {
    if (!user.isAlive()) return false;

    const auto position = field.getRobotPosition(&user);
    if (!position.has_value()) return false;
    if (!user.consumeEnergy(getEnergyCost())) return false;

    int earnedExp = 0;
    for (const auto& [point, robot] : field.getRobots()) {
        if (robot.get() == &user || !robot->isAlive()) continue;
        if (position->distance(point) > radius_) continue;

        robot->takeDamage(Damage);
        if (!robot->isAlive()) {
            earnedExp += 50 * robot->getRank();
        } else {
            robot->addStatus(std::make_unique<Overload>());
        }
    }
    user.addExp(earnedExp);
    return true;
}

void AreaAttack::upgrade() {
    ++radius_;
}
