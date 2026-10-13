#pragma once
#include "Field.h"
#include "Robot.h"

class ConsoleRenderer {
public:
    void render(const Field& field, const Robot& player, int turnNumber) const;
};