#include "GameManager.h"
#include <iostream>
#include <conio.h>

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

        field_->updateFogOfWar(playerPosOpt.value(), player_->getViewRadius());

        renderer_.render(*field_, *player_, turnNumber_);

        processPlayerTurn();

        processEnemiesTurn();

        processFactories();

        endTurn();
    }

    renderer_.render(*field_, *player_, turnNumber_);
    if (state_ == GameState::PlayerWon) {
        std::cout << "\n>>> VICTORY! All enemy robots have been destroyed! <<<\n";
    } else {
        std::cout << "\n>>> DEFEAT! Your robot has been destroyed! <<<\n";
    }
}

void GameManager::processPlayerTurn() {
    auto posOpt = field_->getRobotPosition(player_);
    if (!posOpt.has_value()) return;
    Point curPos = posOpt.value();

    std::cout << "\nYour turn (W/A/S/D - move/attack, any other key - skip turn): ";
    std::cout << std::flush;
    int input = _getch();
    if (input == 0 || input == 224) {
        _getch();
        return;
    }

    Point targetPos = curPos;
    if (input == 'w' || input == 'W') targetPos.y--;
    else if (input == 's' || input == 'S') targetPos.y++;
    else if (input == 'a' || input == 'A') targetPos.x--;
    else if (input == 'd' || input == 'D') targetPos.x++;
    else return;

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
            Point tl = factory->getTopLeft();
            bool spawned = false;

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
    for (const auto& [pos, robot] : field_->getRobots()) {
        if (robot->isAlive()) {
            robot->restoreEnergy(10);
        }
    }

    if (!player_->isAlive()) {
        state_ = GameState::PlayerLost;
        return;
    }

    field_->cleanupDeadRobots();

    int aliveEnemies = 0;
    for (const auto& [pos, robot] : field_->getRobots()) {
        if (!robot->isPlayer() && robot->isAlive()) {
            aliveEnemies++;
        }
    }

    if (aliveEnemies == 0) {
        state_ = GameState::PlayerWon;
    }

    turnNumber_++;
}
