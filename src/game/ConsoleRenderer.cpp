#include "ConsoleRenderer.h"
#include <iostream>
#include <string>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace {
void clearConsole() {
    const HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(output, &info)) return;

    const COORD origin{0, 0};
    const DWORD size = static_cast<DWORD>(info.dwSize.X) * info.dwSize.Y;
    DWORD written;
    FillConsoleOutputCharacterA(output, ' ', size, origin, &written);
    FillConsoleOutputAttribute(output, info.wAttributes, size, origin, &written);
    SetConsoleCursorPosition(output, origin);
}
}

void ConsoleRenderer::render(const Field& field, const Robot& player, int turnNumber) const {
    auto playerPosOpt = field.getRobotPosition(&player);

    std::string buffer;
    buffer += "\n========================================\n";
    buffer += "Turn: " + std::to_string(turnNumber) + "\n";
    buffer += "HP: " + std::to_string(player.getHp()) + "/" + std::to_string(player.getMaxHp()) +
              " | Energy: " + std::to_string(player.getEnergy()) + "/" + std::to_string(player.getMaxEnergy()) +
              " | Rank: " + std::to_string(player.getRank()) +
              " | XP: " + std::to_string(player.getExp()) + "/" + std::to_string(player.getExpToNextRank()) + "\n";
    buffer += "========================================\n";

    for (int y = 0; y < field.getHeight(); ++y) {
        for (int x = 0; x < field.getWidth(); ++x) {
            Point pt{x, y};
            const Cell& cell = field.getCell(pt);

            bool isCurrentlyVisible = (playerPosOpt.has_value() &&
                           playerPosOpt.value().distance(pt) <= player.getViewRadius());

            if (!isCurrentlyVisible && !cell.isDiscovered()) {
                buffer += " ? ";
                continue;
            }

            if (const Robot* robot = field.getRobotAt(pt)) {
                buffer += robot->isPlayer() ? " P " : " E ";
                continue;
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

    clearConsole();
    std::cout << buffer << std::flush;
}
