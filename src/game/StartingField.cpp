#include "StartingField.h"

#include <utility>
#include "Slowness.h"
#include "Overload.h"
#include "Burning.h"
#include "Shield.h"
#include "ItemPickupStatus.h"
#include "HealthPack.h"
#include "EnergyBattery.h"
#include "AttackBooster.h"

StartingField createStartingField() {
    auto field = std::make_unique<Field>(12, 12);
    auto player = std::make_unique<Robot>(true, 100, 25, 60, 3, 4);
    Robot* playerPtr = player.get();
    field->addRobot(std::move(player), {1, 1});
    field->addRobot(std::make_unique<Robot>(false, 70, 10, 20, 2, 3), {6, 6});
    field->addFactory(std::make_unique<RobotFactory>(Point{8, 8}, 10));

    field->getCell({3, 2}).setWalkable(false);
    field->getCell({3, 3}).setWalkable(false);
    field->getCell({0, 3}).setMoveCost(2);
    field->getCell({1, 3}).setMoveCost(2);
    field->getCell({2, 3}).setMoveCost(2);

    field->getCell({2, 1}).addStatus(std::make_unique<Slowness>(2));
    field->getCell({0, 1}).addStatus(std::make_unique<Overload>(2));
    field->getCell({8, 0}).addStatus(std::make_unique<Burning>(3, 10));
    field->getCell({1, 2}).addStatus(std::make_unique<Shield>(30));
    field->getCell({0, 2}).addStatus(
        std::make_unique<ItemPickupStatus>(std::make_unique<HealthPack>()));
    field->getCell({2, 2}).addStatus(
        std::make_unique<ItemPickupStatus>(std::make_unique<EnergyBattery>()));
    field->getCell({2, 0}).addStatus(
        std::make_unique<ItemPickupStatus>(std::make_unique<AttackBooster>()));
    field->updateFogOfWar({1, 1}, playerPtr->getViewRadius());

    return {std::move(field), playerPtr};
}
