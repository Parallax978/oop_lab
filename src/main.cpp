#include <iostream>
#include <array>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include "Field.h"
#include "Robot.h"
#include "RobotFactory.h"
#include "GameManager.h"
#include "AreaAttack.h"
#include "RangedAttack.h"
#include "Healing.h"
#include "Teleportation.h"
#include "Slowness.h"
#include "Overload.h"
#include "Burning.h"
#include "Shield.h"

namespace {
std::unique_ptr<IAbility> selectStartingAbility(int energy) {
    std::array<std::unique_ptr<IAbility>, 4> abilities{
        std::make_unique<AreaAttack>(),
        std::make_unique<RangedAttack>(),
        std::make_unique<Healing>(),
        std::make_unique<Teleportation>()
    };

    std::cout << "Choose one starting ability:\n";
    for (std::size_t index = 0; index < abilities.size(); ++index) {
        const auto& ability = abilities[index];
        std::cout << index + 1 << ". " << ability->getName()
                  << " | Energy: " << ability->getEnergyCost()
                  << " | Range: " << ability->getRange()
                  << (energy >= ability->getEnergyCost()
                      ? " | Available\n" : " | Not enough energy\n");
    }

    std::string line;
    while (true) {
        std::cout << "Enter ability number (1-4) and press Enter: " << std::flush;
        if (!std::getline(std::cin, line)) return nullptr;

        std::istringstream input(line);
        std::size_t choice;
        if (!(input >> choice) || !(input >> std::ws).eof() ||
            choice < 1 || choice > abilities.size()) {
            std::cout << "Invalid choice. Enter a number from 1 to 4.\n";
            continue;
        }
        if (energy < abilities[choice - 1]->getEnergyCost()) {
            std::cout << "Not enough energy for this ability. Choose another one.\n";
            continue;
        }

        std::cout << "Selected: " << abilities[choice - 1]->getName() << "\n";
        return std::move(abilities[choice - 1]);
    }
}
}

int main() {
    try {
        auto field = std::make_unique<Field>(12, 12);

        auto playerUnique = std::make_unique<Robot>(true, 100, 25, 60, 3, 4);
        auto startingAbility = selectStartingAbility(playerUnique->getEnergy());
        if (!startingAbility) return 0;
        playerUnique->addAbility(std::move(startingAbility));
        Robot* playerPtr = playerUnique.get();
        field->addRobot(std::move(playerUnique), Point{1, 1});

        field->addRobot(std::make_unique<Robot>(false, 70, 10, 20, 2, 3), Point{6, 6});

        field->addFactory(std::make_unique<RobotFactory>(Point{8, 8}, 10));

        field->getCell(Point{3, 2}).setWalkable(false);
        field->getCell(Point{3, 3}).setWalkable(false);

        field->getCell(Point{0, 3}).setMoveCost(2);
        field->getCell(Point{1, 3}).setMoveCost(2);
        field->getCell(Point{2, 3}).setMoveCost(2);

        field->getCell(Point{2, 1}).addStatus(std::make_unique<Slowness>(2));
        field->getCell(Point{0, 1}).addStatus(std::make_unique<Overload>(2));
        field->getCell(Point{8, 0}).addStatus(std::make_unique<Burning>(3, 10));
        field->getCell(Point{1, 2}).addStatus(std::make_unique<Shield>(30));

        GameManager game(std::move(field), playerPtr);
        game.run();

    } catch (const std::exception& ex) {
        std::cerr << "Critical error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
