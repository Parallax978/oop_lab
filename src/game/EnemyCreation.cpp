#include "EnemyCreation.h"

#include <random>
#include "Healing.h"
#include "Teleportation.h"

std::unique_ptr<Robot> createEnemyRobot(int attack, int maxEnergy) {
    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> abilityChoice(0, 1);

    auto enemy = std::make_unique<Robot>(false, 70, attack, maxEnergy, 2, 3);
    if (abilityChoice(generator) == 0) {
        enemy->addAbility(std::make_unique<Healing>());
    } else {
        enemy->addAbility(std::make_unique<Teleportation>());
    }
    return enemy;
}
