#pragma once
#include "../core/Field.h"
#include "../entities/Robot.h"

class ConsoleRenderer {
public:
    void render(const Field& field, const Robot& player, int turnNumber) const;
};