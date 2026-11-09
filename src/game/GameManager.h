#pragma once
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
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
    void processPlayerAbility(std::size_t index, int movesLeft, int maxMoves);
    std::optional<Point> selectAbilityTarget(Point start, const IAbility& ability,
                                             int movesLeft, int maxMoves);
    void processEnemiesTurn();
    void processFactories();
    void endTurn();
    void checkGameState();
    void processRankUpChoices();
    void renderFrame(int movesLeft = -1, int maxMoves = 0,
                     std::optional<Point> selectedTarget = std::nullopt);
    bool canPlayerAct(int movesLeft) const;

    std::unique_ptr<Field> field_;
    Robot* player_;
    ConsoleRenderer renderer_;
    GameState state_{GameState::InProgress};
    int turnNumber_{1};
    int lastRewardedRank_;
    bool showStatusCells_{true};
    std::string message_;
};
