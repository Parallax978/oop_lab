#pragma once
#include <optional>
#include <string>
#include "Field.h"
#include "Robot.h"

class ConsoleRenderer {
public:
    void render(const Field& field, const Robot& player, int turnNumber,
                int movesLeft = -1, int maxMoves = 0, const std::string& message = "",
                std::optional<Point> selectedTarget = std::nullopt) const;
};
