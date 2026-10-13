#include "GameManager.h"
#include <iostream>
#include <cmath>

GameManager::GameManager(std::unique_ptr<Field> field, Robot* player)
    : field_(std::move(field)), player_(player) {}

GameState GameManager::getState() const {
    return state_;
}

void GameManager::run() {
    while (state_ == GameState::InProgress) {
        auto playerPosOpt = field_->getRobotPosition(player_);
        if (!playerPosOpt.has_value() || !player_->isAlive()) {
            state_ = GameState::PlayerLost;
            break;
        }

        // Обновляем туман войны перед отображением
        field_->updateFogOfWar(playerPosOpt.value(), player_->getViewRadius());

        // Отрисовываем кадр
        renderer_.render(*field_, *player_, turnNumber_);

        // 1. Ход игрока
        processPlayerTurn();

        // 2. Ход врагов
        processEnemiesTurn();

        // 3. Работа заводов
        processFactories();

        // 4. Окончание хода (регенерация энергии, очистка, проверка победы)
        endTurn();
    }

    // Финальный экран
    auto finalPos = field_->getRobotPosition(player_).value_or(Point{0, 0});
    renderer_.render(*field_, *player_, turnNumber_);
    if (state_ == GameState::PlayerWon) {
        std::cout << "\n>>> ПОБЕДА! Все вражеские роботы уничтожены! <<<\n";
    } else {
        std::cout << "\n>>> ПОРАЖЕНИЕ! Робот игрока уничтожен! <<<\n";
    }
}

void GameManager::processPlayerTurn() {
    auto posOpt = field_->getRobotPosition(player_);
    if (!posOpt.has_value()) return;
    Point curPos = posOpt.value();

    std::cout << "\nВаш ход (W/A/S/D — движение/атака, Пропуск хода — любая другая клавиша): ";
    char input;
    if (!(std::cin >> input)) return;

    Point targetPos = curPos;
    if (input == 'w' || input == 'W') targetPos.y--;
    else if (input == 's' || input == 'S') targetPos.y++;
    else if (input == 'a' || input == 'A') targetPos.x--;
    else if (input == 'd' || input == 'D') targetPos.x++;
    else return; // Пропуск хода

    // Учитываем проходимость и затраты на шаг
    if (field_->isInside(targetPos)) {
        field_->moveRobot(player_, targetPos);
    }
}

void GameManager::processEnemiesTurn() {
    auto playerPosOpt = field_->getRobotPosition(player_);
    if (!playerPosOpt.has_value()) return;
    Point playerPos = playerPosOpt.value();

    for (const auto& [pos, robot] : field_->getRobots()) {
        if (!robot->isPlayer() && robot->isAlive()) {
            Point enemyPos = pos;
            Point step = enemyPos;

            // Простейший поиск пути в сторону игрока
            if (playerPos.x > enemyPos.x) step.x++;
            else if (playerPos.x < enemyPos.x) step.x--;
            else if (playerPos.y > enemyPos.y) step.y++;
            else if (playerPos.y < enemyPos.y) step.y--;

            field_->moveRobot(robot.get(), step);
        }
    }
}

void GameManager::processFactories() {
    for (const auto& factory : field_->getFactories()) {
        if (factory->tick()) {
            // Завод создает нового врага рядом с собой на свободной клетке
            Point tl = factory->getTopLeft();
            bool spawned = false;

            // Ищем первую свободную клетку вокруг завода 2х2
            for (int dy = -1; dy <= 2 && !spawned; ++dy) {
                for (int dx = -1; dx <= 2 && !spawned; ++dx) {
                    Point spawnPt{tl.x + dx, tl.y + dy};
                    if (field_->isWalkable(spawnPt) && field_->getRobotAt(spawnPt) == nullptr) {
                        auto enemy = std::make_unique<Robot>(false, 60, 15, 30, 2, 3);
                        field_->addRobot(std::move(enemy), spawnPt);
                        spawned = true;
                    }
                }
            }
        }
    }
}

void GameManager::endTurn() {
    // В конце хода каждый робот восстанавливает часть энергии (требование ТЗ)
    for (const auto& [pos, robot] : field_->getRobots()) {
        if (robot->isAlive()) {
            robot->restoreEnergy(10);
        }
    }

    // Удаление уничтоженных роботов из вектора
    field_->cleanupDeadRobots();

    // Проверка условий завершения игры
    if (!player_->isAlive()) {
        state_ = GameState::PlayerLost;
        return;
    }

    // Подсчет оставшихся врагов
    int aliveEnemies = 0;
    for (const auto& [pos, robot] : field_->getRobots()) {
        if (!robot->isPlayer() && robot->isAlive()) {
            aliveEnemies++;
        }
    }

    // Победа засчитывается, если врагов нет и заводы отсутствуют/выключены
    if (aliveEnemies == 0 && field_->getFactories().empty()) {
        state_ = GameState::PlayerWon;
    }

    turnNumber_++;
}