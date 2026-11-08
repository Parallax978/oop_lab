#include <iostream>
#include <memory>
#include "Field.h"
#include "Robot.h"
#include "RobotFactory.h"
#include "GameManager.h"
#include "AreaAttack.h"
#include "RangedAttack.h"
#include "Healing.h"
#include "Teleportation.h"

int main() {
    try {
        auto field = std::make_unique<Field>(12, 12);

        auto playerUnique = std::make_unique<Robot>(true, 100, 25, 60, 3, 4);
        playerUnique->addAbility(std::make_unique<AreaAttack>());
        playerUnique->addAbility(std::make_unique<RangedAttack>());
        playerUnique->addAbility(std::make_unique<Healing>());
        playerUnique->addAbility(std::make_unique<Teleportation>());
        Robot* playerPtr = playerUnique.get();
        field->addRobot(std::move(playerUnique), Point{1, 1});

        field->addRobot(std::make_unique<Robot>(false, 70, 10, 20, 2, 3), Point{6, 6});

        field->addFactory(std::make_unique<RobotFactory>(Point{8, 8}, 10));

        field->getCell(Point{3, 2}).setWalkable(false);
        field->getCell(Point{3, 3}).setWalkable(false);

        field->getCell(Point{0, 3}).setMoveCost(2);
        field->getCell(Point{1, 3}).setMoveCost(2);
        field->getCell(Point{2, 3}).setMoveCost(2);

        GameManager game(std::move(field), playerPtr);
        game.run();

    } catch (const std::exception& ex) {
        std::cerr << "Critical error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
