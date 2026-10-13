#include <iostream>
#include <memory>
#include "core/Field.h"
#include "entities/Robot.h"
#include "RobotFactory.h"
#include "game/GameManager.h"

int main() {
    try {
        // Создаем поле 12х12 (в пределах допустимых границ)
        auto field = std::make_unique<Field>(12, 12);

        // Создаем игрока и добавляем на поле
        auto playerUnique = std::make_unique<Robot>(true, 100, 25, 60, 3, 4);
        Robot* playerPtr = playerUnique.get();
        field->addRobot(std::move(playerUnique), Point{1, 1});

        // Добавляем вражеского робота
        field->addRobot(std::make_unique<Robot>(false, 40, 10, 20, 2, 3), Point{6, 6});

        // Добавляем фабрику 2x2 в правый нижний угол (спавн раз в 3 хода)
        field->addFactory(std::make_unique<RobotFactory>(Point{8, 8}, 3));

        // Добавляем препятствия
        field->getCell(Point{3, 2}).setWalkable(false);
        field->getCell(Point{3, 3}).setWalkable(false);

        // Запуск менеджера игры
        GameManager game(std::move(field), playerPtr);
        game.run();

    } catch (const std::exception& ex) {
        std::cerr << "Критическая ошибка: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}