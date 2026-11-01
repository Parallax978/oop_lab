#include "Field.h"
#include <cassert>
#include <iostream>
#include <memory>

int main() {
    Field field(12, 12);
    auto owned = std::make_unique<Robot>(true, 100, 25, 60);
    Robot* player = owned.get();
    assert(field.addRobot(std::move(owned), {1, 1}));
    assert(field.moveRobot(player, {2, 1}));
    assert(player->getEnergy() == 45);
    assert(field.getRobotAt({2, 1}) == player);

    field.getCell({3, 1}).setWalkable(false);
    assert(!field.moveRobot(player, {3, 1}));
    assert(player->getEnergy() == 45);
    assert(!field.moveRobot(player, {4, 1}));
    assert(!field.moveRobot(player, {2, 1}));
    assert(player->getEnergy() == 45);

    field.getCell({2, 2}).setMoveCost(2);
    assert(field.moveRobot(player, {2, 2}));
    assert(player->getEnergy() == 15);
    field.getCell({2, 3}).setMoveCost(2);
    assert(!field.moveRobot(player, {2, 3}));
    assert(field.getRobotAt({2, 2}) == player);
    assert(player->getEnergy() == 15);

    auto enemyOwned = std::make_unique<Robot>(false, 100);
    Robot* enemy = enemyOwned.get();
    assert(field.addRobot(std::move(enemyOwned), {3, 2}));
    assert(!field.moveRobot(player, {3, 2}));
    assert(enemy->getHp() == 100);
    assert(player->getEnergy() == 15);
    player->restoreEnergy(10);
    assert(field.moveRobot(player, {3, 2}));
    assert(enemy->getHp() == 75);
    assert(player->getEnergy() == 5);
    assert(field.getRobotAt({2, 2}) == player);

    Robot healer(true, 100, 20, 40);
    Robot ally(true);
    ally.takeDamage(50);
    assert(healer.interact(ally));
    assert(ally.getHp() == 70);
    assert(healer.getEnergy() == 20);
    assert(healer.interact(ally));
    assert(healer.getEnergy() == 0);
    assert(!healer.interact(ally));
    assert(ally.getHp() == 90);
    assert(!healer.interact(healer));
    std::cout << "Robot action checks passed\n";
}
