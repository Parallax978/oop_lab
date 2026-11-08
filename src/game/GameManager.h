#pragma once
#include <memory>
#include <string>
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
    void checkGameState();
    void renderFrame(int movesLeft = -1, int maxMoves = 0);
    bool canPlayerAct(int movesLeft) const;

    std::unique_ptr<Field> field_;
    Robot* player_;
    ConsoleRenderer renderer_;
    GameState state_{GameState::InProgress};
    int turnNumber_{1};
    std::string message_;
};
