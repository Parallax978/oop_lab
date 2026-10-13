#pragma once
#include <memory>
#include "../core/Field.h"
#include "../entities/Robot.h"
#include "ConsoleRenderer.h"

enum class GameState {
    InProgress,
    PlayerWon,
    PlayerLost
};

class GameManager {
public:
    GameManager(std::unique_ptr<Field> field, Robot* player);

    // Запуск главного игрового цикла
    void run();

    GameState getState() const;

private:
    // Подшаги одного пошагового раунда
    void processPlayerTurn();
    void processEnemiesTurn();
    void processFactories();
    void endTurn();

    std::unique_ptr<Field> field_;
    Robot* player_; // Не владеющий указатель на робота игрока (живет внутри field_)
    ConsoleRenderer renderer_;
    GameState state_{GameState::InProgress};
    int turnNumber_{1};
};