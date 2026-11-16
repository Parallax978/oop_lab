#include "StartingField.h"
#include "EnemyCreation.h"

#include <algorithm>
#include <random>
#include <utility>
#include <vector>
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
    field->addRobot(createEnemyRobot(10, 20), {6, 6});
    field->addFactory(std::make_unique<RobotFactory>(Point{8, 8}, 10));

    field->getCell({3, 2}).setWalkable(false);
    field->getCell({3, 3}).setWalkable(false);
    field->getCell({0, 3}).setMoveCost(2);
    field->getCell({1, 3}).setMoveCost(2);
    field->getCell({2, 3}).setMoveCost(2);

    field->getCell({0, 2}).addStatus(
        std::make_unique<ItemPickupStatus>(std::make_unique<HealthPack>()));
    field->getCell({2, 2}).addStatus(
        std::make_unique<ItemPickupStatus>(std::make_unique<EnergyBattery>()));
    field->getCell({2, 0}).addStatus(
        std::make_unique<ItemPickupStatus>(std::make_unique<AttackBooster>()));

    std::vector<Point> availableCells;
    for (int y = 0; y < field->getHeight(); ++y) {
        for (int x = 0; x < field->getWidth(); ++x) {
            const Point position{x, y};
            if (field->isWalkable(position) && !field->getRobotAt(position) &&
                field->getCell(position).getStatusCount() == 0) {
                availableCells.push_back(position);
            }
        }
    }

    static std::mt19937 generator(std::random_device{}());
    std::shuffle(availableCells.begin(), availableCells.end(), generator);
    std::uniform_int_distribution<int> statusChoice(0, 3);
    constexpr std::size_t statusCount = 12;
    const std::size_t cellsToFill = std::min(statusCount, availableCells.size());
    for (std::size_t index = 0; index < cellsToFill; ++index) {
        Cell& cell = field->getCell(availableCells[index]);
        switch (statusChoice(generator)) {
        case 0:
            cell.addStatus(std::make_unique<Slowness>(2));
            break;
        case 1:
            cell.addStatus(std::make_unique<Overload>(2));
            break;
        case 2:
            cell.addStatus(std::make_unique<Burning>(3, 10));
            break;
        case 3:
            cell.addStatus(std::make_unique<Shield>(30));
            break;
        }
    }

    field->updateFogOfWar({1, 1}, playerPtr->getViewRadius());

    return {std::move(field), playerPtr};
}
