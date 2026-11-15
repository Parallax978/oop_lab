#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include "Field.h"

class GraphicsRenderer {
public:
    void render(sf::RenderTarget& target, const Field& field, const Robot& player,
                bool showStatusCells = true,
                std::optional<Point> selectedTarget = std::nullopt) const;
};
