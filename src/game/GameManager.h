#pragma once
#include <memory>
#include "Field.h"
#include "Robot.h"
#include "ConsoleRenderer.h"

enum class GameState {
    InProgress,
    PlayerWon,
    PlayerLost
};

class GameManager {
public:
    GameManager(std::unique_ptr<Field> field, Robot* player);
    void run();

    GameState getState() const;

private:
    void processPlayerTurn();
    void processEnemiesTurn();
    void processFactories();
    void endTurn();

    std::unique_ptr<Field> field_;
    Robot* player_;
    ConsoleRenderer renderer_;
    GameState state_{GameState::InProgress};
    int turnNumber_{1};
};