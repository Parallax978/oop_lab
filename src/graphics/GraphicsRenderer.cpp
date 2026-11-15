#include "GraphicsRenderer.h"

#include <algorithm>
#include <SFML/Graphics.hpp>
#include <string_view>

namespace {
void rectangle(sf::RenderTarget& target, sf::Vector2f position,
               sf::Vector2f size, sf::Color color) {
    sf::RectangleShape shape(size);
    shape.setPosition(position);
    shape.setFillColor(color);
    target.draw(shape);
}

sf::Color statusColor(std::string_view name) {
    if (name == "Slowness") return {240, 200, 75};
    if (name == "Overload") return {175, 115, 235};
    if (name == "Burning") return {250, 130, 65};
    if (name == "Shield") return {85, 165, 245};
    return {235, 130, 195};
}

void drawRobot(sf::RenderTarget& target, const Robot& robot,
               sf::Vector2f origin, float size) {
    const sf::Color body = robot.isPlayer() ? sf::Color(65, 205, 180)
                                          : sf::Color(230, 95, 105);
    rectangle(target, origin + sf::Vector2f(size * .22f, size * .23f),
              {size * .56f, size * .48f}, body);
    rectangle(target, origin + sf::Vector2f(size * .31f, size * .34f),
              {size * .38f, size * .10f}, {20, 35, 48});
    rectangle(target, origin + sf::Vector2f(size * .14f, size * .38f),
              {size * .06f, size * .25f}, body);
    rectangle(target, origin + sf::Vector2f(size * .80f, size * .38f),
              {size * .06f, size * .25f}, body);
    rectangle(target, origin + sf::Vector2f(size * .22f, size * .80f),
              {size * .56f, size * .05f}, {20, 28, 38});
    const float hpRatio = std::clamp(static_cast<float>(robot.getHp()) /
                                    robot.getMaxHp(), 0.f, 1.f);
    rectangle(target, origin + sf::Vector2f(size * .22f, size * .80f),
              {size * .56f * hpRatio, size * .05f}, body);
}
}

void GraphicsRenderer::render(sf::RenderTarget& target, const Field& field,
                              const Robot& player, bool showStatusCells,
                              std::optional<Point> selectedTarget) const {
    const auto playerPosition = field.getRobotPosition(&player);
    const sf::Vector2f viewSize = target.getView().getSize();
    if (viewSize.x <= 0.f || viewSize.y <= 0.f) return;
    const float margin = std::min({24.f, viewSize.x * .04f, viewSize.y * .04f});
    const float cellSize = std::min((viewSize.x - 2.f * margin) / field.getWidth(),
                                    (viewSize.y - 2.f * margin) / field.getHeight());
    const sf::Vector2f boardSize{cellSize * field.getWidth(), cellSize * field.getHeight()};
    const sf::Vector2f boardOrigin = target.getView().getCenter() - boardSize / 2.f;
    const float gap = cellSize * .025f;

    for (int y = 0; y < field.getHeight(); ++y) {
        for (int x = 0; x < field.getWidth(); ++x) {
            const Point point{x, y};
            const Cell& cell = field.getCell(point);
            const sf::Vector2f origin = boardOrigin +
                sf::Vector2f(cellSize * x, cellSize * y);
            const sf::Vector2f tileOrigin = origin + sf::Vector2f(gap, gap);
            const sf::Vector2f tileSize{cellSize - 2.f * gap, cellSize - 2.f * gap};
            const bool visible = playerPosition &&
                playerPosition->distance(point) <= player.getViewRadius();

            if (!visible && !cell.isDiscovered()) {
                rectangle(target, tileOrigin, tileSize, {23, 29, 40});
                continue;
            }

            const sf::Color ground = cell.getMoveCost() > 1 ? sf::Color(100, 77, 53)
                : (x + y) % 2 == 0 ? sf::Color(46, 62, 76) : sf::Color(42, 57, 71);
            rectangle(target, tileOrigin, tileSize, ground);

            if (field.getFactoryAt(point)) {
                rectangle(target, origin + sf::Vector2f(cellSize * .09f, cellSize * .09f),
                          {cellSize * .82f, cellSize * .82f}, {165, 110, 65});
                rectangle(target, origin + sf::Vector2f(cellSize * .22f, cellSize * .25f),
                          {cellSize * .56f, cellSize * .14f}, {245, 183, 85});
            } else if (!cell.isWalkable()) {
                rectangle(target, origin + sf::Vector2f(cellSize * .07f, cellSize * .07f),
                          {cellSize * .86f, cellSize * .86f}, {112, 128, 145});
                rectangle(target, origin + sf::Vector2f(cellSize * .16f, cellSize * .16f),
                          {cellSize * .68f, cellSize * .12f}, {150, 164, 177});
            } else if (cell.getMoveCost() > 1) {
                for (int stripe = 0; stripe < 3; ++stripe) {
                    rectangle(target, origin + sf::Vector2f(cellSize * .18f,
                              cellSize * (.27f + stripe * .20f)),
                              {cellSize * .64f, cellSize * .07f}, {155, 120, 75});
                }
            }

            if (showStatusCells && cell.isWalkable() && !field.getFactoryAt(point)) {
                const float spacing = cellSize / (static_cast<float>(cell.getStatusCount()) + 1.f);
                for (std::size_t index = 0; index < cell.getStatusCount(); ++index) {
                    const IStatus* status = cell.getStatus(index);
                    if (!status || status->isExpired()) continue;
                    const float radius = std::min(cellSize * .19f, spacing * .4f);
                    sf::CircleShape diamond(radius, 4);
                    diamond.setOrigin({radius, radius});
                    diamond.setPosition(origin + sf::Vector2f(
                        spacing * static_cast<float>(index + 1), cellSize * .5f));
                    diamond.setFillColor(statusColor(status->getName()));
                    target.draw(diamond);
                }
            }

            if (visible) {
                if (const Robot* robot = field.getRobotAt(point)) {
                    drawRobot(target, *robot, origin, cellSize);
                }
            } else {
                rectangle(target, tileOrigin, tileSize, {0, 0, 0, 155});
            }
        }
    }
    if (selectedTarget && field.isInside(*selectedTarget)) {
        sf::RectangleShape selection({cellSize - 2.f * gap, cellSize - 2.f * gap});
        selection.setPosition(boardOrigin + sf::Vector2f(
            cellSize * selectedTarget->x + gap, cellSize * selectedTarget->y + gap));
        selection.setFillColor(sf::Color::Transparent);
        selection.setOutlineColor(sf::Color(250, 200, 80));
        selection.setOutlineThickness(-cellSize * .06f);
        target.draw(selection);
    }
}
