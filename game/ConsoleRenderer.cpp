#include "../ConsoleRenderer.h"
#include <iostream>
#include <string>

void ConsoleRenderer::render(const Field& field, const Robot& player, int turnNumber) const {
    auto playerPosOpt = field.getRobotPosition(&player);
    Point playerPos = playerPosOpt.value_or(Point{-1, -1});

    std::string buffer;
    buffer += "\n========================================\n";
    buffer += "Ход: " + std::to_string(turnNumber) + "\n";
    buffer += "HP: " + std::to_string(player.getHp()) + "/" + std::to_string(player.getMaxHp()) +
              " | Энергия: " + std::to_string(player.getEnergy()) + "/" + std::to_string(player.getMaxEnergy()) +
              " | Ранг: " + std::to_string(player.getRank()) +
              " | Опыт: " + std::to_string(player.getExp()) + "/" + std::to_string(player.getExpToNextRank()) + "\n";
    buffer += "========================================\n";

    for (int y = 0; y < field.getHeight(); ++y) {
        for (int x = 0; x < field.getWidth(); ++x) {
            Point pt{x, y};
            const Cell& cell = field.getCell(pt);

            bool isCurrentlyVisible = (playerPosOpt.has_value() && player.canSee(pt));

            if (!isCurrentlyVisible && !cell.isDiscovered()) {
                buffer += " ? ";
                continue;
            }

            if (isCurrentlyVisible) {
                if (const Robot* robot = field.getRobotAt(pt)) {
                    buffer += robot->isPlayer() ? " P " : " E ";
                    continue;
                }
            }

            if (field.getFactoryAt(pt) != nullptr) {
                buffer += " F ";
            } else if (!cell.isWalkable()) {
                buffer += " # ";
            } else if (cell.getMoveCost() > 1) {
                buffer += " ~ ";
            } else {
                buffer += " . ";
            }
        }
        buffer += "\n";
    }

    std::cout << buffer;
}