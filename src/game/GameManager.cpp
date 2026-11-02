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
        // Some terminals provide a Unicode character without a virtual key code.
        switch (key.uChar.UnicodeChar) {
        case L'w': case L'W': case L'\u0446': case L'\u0426': return 'W';
        case L'a': case L'A': case L'\u0444': case L'\u0424': return 'A';
        case L's': case L'S': case L'\u044b': case L'\u042b': return 'S';
        case L'd': case L'D': case L'\u0432': case L'\u0412': return 'D';
        case L'\r': case L'\n': return VK_RETURN;
        default: return key.wVirtualKeyCode;
        }
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

    if (auto position = field_->getRobotPosition(player_); position.has_value()) {
        field_->updateFogOfWar(position.value(), player_->getViewRadius());
    }
    renderer_.render(*field_, *player_, turnNumber_);
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
    auto posOpt = field_->getRobotPosition(player_);
    if (!posOpt.has_value()) return;
    Point curPos = posOpt.value();

    std::cout << "\nYour turn (W/A/S/D - move/attack, any other key - skip turn): ";
    std::cout << std::flush;
    const WORD input = readKey();

    Point targetPos = curPos;
    if (input == 'W') targetPos.y--;
    else if (input == 'S') targetPos.y++;
    else if (input == 'A') targetPos.x--;
    else if (input == 'D') targetPos.x++;
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
