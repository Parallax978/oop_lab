#include "GameManager.h"
#include <iostream>
#include <stdexcept>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace {
WORD readKey() {
    const HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD event;
    DWORD count;
    while (ReadConsoleInputW(input, &event, 1, &count)) {
        if (count == 0 || event.EventType != KEY_EVENT) continue;
        const auto& key = event.Event.KeyEvent;
        if (!key.bKeyDown) continue;
        if (key.wVirtualKeyCode == VK_SHIFT || key.wVirtualKeyCode == VK_CONTROL ||
            key.wVirtualKeyCode == VK_MENU || key.wVirtualKeyCode == VK_CAPITAL) continue;
        return key.wVirtualKeyCode;
    }
    throw std::runtime_error("Cannot read keyboard input. Run the game in a Windows console.");
}
}

GameManager::GameManager(std::unique_ptr<Field> field, Robot* player)
    : field_(std::move(field)), player_(player) {}

GameState GameManager::getState() const {
    return state_;
}

void GameManager::run() {
    while (state_ == GameState::InProgress) {
        checkGameState();
        if (state_ != GameState::InProgress) break;
        processPlayerTurn();
        if (state_ != GameState::InProgress) break;
        processEnemiesTurn();
        if (state_ != GameState::InProgress) break;
        processFactories();
        endTurn();
    }

    renderFrame();
    if (state_ == GameState::PlayerWon) {
        std::cout << "\n>>> VICTORY! All enemy robots have been destroyed! <<<\n";
    } else {
        std::cout << "\n>>> DEFEAT! Your robot has been destroyed! <<<\n";
    }
    std::cout << "\nPress Enter to exit..." << std::flush;
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    while (readKey() != VK_RETURN) {
    }
}

void GameManager::processPlayerTurn() {
    const int maxMoves = player_->getSpeed();
    int movesLeft = maxMoves;
    while (state_ == GameState::InProgress) {
        if (!canPlayerAct(movesLeft)) {
            message_ = "No available actions. Your turn has ended.";
            renderFrame(movesLeft, maxMoves);
            return;
        }
        renderFrame(movesLeft, maxMoves);
        const WORD input = readKey();
        if (input == VK_SPACE) {
            message_.clear();
            return;
        }
        auto position = field_->getRobotPosition(player_);
        if (!position.has_value()) return;
        if (input == '1' || input == VK_NUMPAD1) {
            const IAbility* ability = player_->getAbility(0);
            if (!ability) {
                message_ = "You don't have an ability in this slot";
            } else if (player_->getEnergy() < ability->getEnergyCost()) {
                message_ = "You don't have enough energy for this ability";
            } else if (player_->useAbility(0, *field_, position.value())) {
                message_ = std::string(ability->getName()) + " used";
                checkGameState();
                field_->cleanupDeadRobots();
            } else {
                message_ = "Cannot use this ability";
            }
            continue;
        }
        Point target = position.value();
        if (input == 'W') --target.y;
        else if (input == 'S') ++target.y;
        else if (input == 'A') --target.x;
        else if (input == 'D') ++target.x;
        else continue;

        const ActionResult result = field_->moveRobot(player_, target, movesLeft > 0);
        switch (result) {
        case ActionResult::Moved:
            --movesLeft;
            message_.clear();
            break;
        case ActionResult::Interacted:
            message_ = "Interaction completed. Your turn has ended.";
            checkGameState();
            renderFrame(movesLeft, maxMoves);
            return;
        case ActionResult::Blocked:
            message_ = "You can't move in this direction";
            break;
        case ActionResult::NoEnergy:
            message_ = "You don't have enough energy for this action";
            break;
        case ActionResult::NoMoves:
            message_ = "You have no moves left";
            break;
        }
    }
}

void GameManager::processEnemiesTurn() {
    for (const auto& [pos, robot] : field_->getRobots()) {
        if (!robot->isPlayer() && robot->isAlive()) {
            int movesLeft = robot->getSpeed();
            while (state_ == GameState::InProgress) {
                const auto playerPosition = field_->getRobotPosition(player_);
                const auto enemyPosition = field_->getRobotPosition(robot.get());
                if (!playerPosition || !enemyPosition) return;
                const Point playerPos = playerPosition.value();
                const Point enemyPos = enemyPosition.value();
                Point step = enemyPos;
                if (playerPos.x > enemyPos.x) ++step.x;
                else if (playerPos.x < enemyPos.x) --step.x;
                else if (playerPos.y > enemyPos.y) ++step.y;
                else if (playerPos.y < enemyPos.y) --step.y;

                const ActionResult result = field_->moveRobot(robot.get(), step, movesLeft > 0);
                if (result == ActionResult::Moved) {
                    --movesLeft;
                    message_ = "Enemy turn";
                } else if (result == ActionResult::Interacted) {
                    message_ = "Enemy interaction completed";
                    checkGameState();
                } else {
                    break;
                }
                renderFrame();
                Sleep(100);
                if (result == ActionResult::Interacted) break;
            }
            if (state_ != GameState::InProgress) return;
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
                        auto enemy = std::make_unique<Robot>(false, 70, 15, 30, 2, 3);
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

    checkGameState();
    if (state_ != GameState::InProgress) return;
    field_->cleanupDeadRobots();
    turnNumber_++;
}

void GameManager::checkGameState() {
    if (!field_->getRobotPosition(player_).has_value() || !player_->isAlive()) {
        state_ = GameState::PlayerLost;
        return;
    }
    for (const auto& [pos, robot] : field_->getRobots()) {
        if (!robot->isPlayer() && robot->isAlive()) {
            return;
        }
    }
    state_ = GameState::PlayerWon;
}

void GameManager::renderFrame(int movesLeft, int maxMoves) {
    if (auto position = field_->getRobotPosition(player_); position.has_value()) {
        field_->updateFogOfWar(position.value(), player_->getViewRadius());
    }
    renderer_.render(*field_, *player_, turnNumber_, movesLeft, maxMoves, message_);
}

bool GameManager::canPlayerAct(int movesLeft) const {
    const auto position = field_->getRobotPosition(player_);
    if (!position || !player_->isAlive()) return false;
    const IAbility* ability = player_->getAbility(0);
    if (ability && player_->getEnergy() >= ability->getEnergyCost()) return true;
    constexpr Point directions[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    for (const Point direction : directions) {
        const Point target{position->x + direction.x, position->y + direction.y};
        if (!field_->isWalkable(target)) continue;
        if (field_->getRobotAt(target)) {
            if (player_->getEnergy() >= Robot::InteractionEnergyCost) return true;
        } else if (movesLeft > 0 && field_->getCell(target).getMoveCost() <=
                   player_->getEnergy() / player_->getMoveEnergyCost()) {
            return true;
        }
    }
    return false;
}
