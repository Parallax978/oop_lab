#pragma once

#include <memory>
#include "Field.h"

struct StartingField {
    std::unique_ptr<Field> field;
    Robot* player;
};

StartingField createStartingField();
