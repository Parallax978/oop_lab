#include "ConsoleRenderer.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace {
template <typename Owner>
std::string describeStatuses(const Owner& owner) {
    std::string result;
    for (std::size_t index = 0; index < owner.getStatusCount(); ++index) {
        const IStatus* status = owner.getStatus(index);
        if (!status || status->isExpired()) continue;
        if (!result.empty()) result += ", ";
        result += status->getDescription();
    }
    return result.empty() ? "None" : result;
}

void clearConsole() {
    const HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD originalMode;
    if (GetConsoleMode(output, &originalMode) &&
        SetConsoleMode(output, originalMode | ENABLE_PROCESSED_OUTPUT |
                               ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        std::cout << "\x1b[2J\x1b[3J\x1b[H" << std::flush;
        SetConsoleMode(output, originalMode);
        return;
    }

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

void ConsoleRenderer::initializeConsole() const {
    const HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(output, &info)) return;

    const SHORT currentHeight = info.srWindow.Bottom - info.srWindow.Top + 1;
    const COORD maximumWindow = GetLargestConsoleWindowSize(output);
    const SHORT desiredHeight = std::min<SHORT>(50, maximumWindow.Y);
    if (desiredHeight <= currentHeight) return;

    COORD bufferSize = info.dwSize;
    bufferSize.Y = std::max<SHORT>(bufferSize.Y, info.srWindow.Top + desiredHeight);
    if (bufferSize.Y != info.dwSize.Y &&
        !SetConsoleScreenBufferSize(output, bufferSize)) return;

    SMALL_RECT window = info.srWindow;
    window.Bottom = window.Top + desiredHeight - 1;
    SetConsoleWindowInfo(output, TRUE, &window);
}

void ConsoleRenderer::render(const Field& field, const Robot& player, int turnNumber,
                             int movesLeft, int maxMoves, const std::string& message,
                             std::optional<Point> selectedTarget) const {
    auto playerPosOpt = field.getRobotPosition(&player);

    std::string buffer;
    struct ColorRange {
        std::size_t begin;
        std::size_t end;
        bool isTarget;
    };
    std::vector<ColorRange> colorRanges;
    buffer += "\n========================================\n";
    buffer += "Turn: " + std::to_string(turnNumber) + "\n";
    buffer += "HP: " + std::to_string(player.getHp()) + "/" + std::to_string(player.getMaxHp()) +
              " | Energy: " + std::to_string(player.getEnergy()) + "/" + std::to_string(player.getMaxEnergy()) +
              " | Rank: " + std::to_string(player.getRank()) +
              " | XP: " + std::to_string(player.getExp()) + "/" + std::to_string(player.getExpToNextRank()) + "\n";
    buffer += "Statuses: " + describeStatuses(player) + "\n";
    if (!player.canMove() && player.isAlive()) buffer += "Movement blocked by a status\n";
    buffer += "========================================\n";

    for (int y = 0; y < field.getHeight(); ++y) {
        for (int x = 0; x < field.getWidth(); ++x) {
            Point pt{x, y};
            const Cell& cell = field.getCell(pt);
            const bool isTarget = selectedTarget && selectedTarget.value() == pt;
            if (isTarget) {
                colorRanges.push_back({buffer.size(), buffer.size() + 3, true});
            }

            bool isCurrentlyVisible = (playerPosOpt.has_value() &&
                           playerPosOpt.value().distance(pt) <= player.getViewRadius());

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

            const std::size_t cellStart = buffer.size();
            if (field.getFactoryAt(pt) != nullptr) {
                buffer += " F ";
            } else if (!cell.isWalkable()) {
                buffer += " # ";
            } else if (cell.getStatusCount() > 0) {
                buffer += " ! ";
            } else if (cell.getMoveCost() > 1) {
                buffer += " ~ ";
            } else {
                buffer += " . ";
            }
            if (!isCurrentlyVisible && !isTarget) {
                if (!colorRanges.empty() && !colorRanges.back().isTarget &&
                    colorRanges.back().end == cellStart) {
                    colorRanges.back().end = buffer.size();
                } else {
                    colorRanges.push_back({cellStart, buffer.size(), false});
                }
            }
        }
        buffer += "\n";
    }

    buffer += "\n! - one-use status cell; ~ - difficult terrain\n";
    if (playerPosOpt) {
        buffer += "Current cell effects: " +
                  describeStatuses(field.getCell(*playerPosOpt)) + "\n";
        for (const auto& [position, robot] : field.getRobots()) {
            if (robot->isPlayer() || !robot->isAlive() || robot->getStatusCount() == 0 ||
                playerPosOpt->distance(position) > player.getViewRadius()) continue;
            buffer += "Enemy (" + std::to_string(position.x) + ", " +
                      std::to_string(position.y) + "): " + describeStatuses(*robot) + "\n";
        }
    }
    if (selectedTarget && field.isInside(*selectedTarget)) {
        const Cell& cell = field.getCell(*selectedTarget);
        const bool visible = playerPosOpt &&
            playerPosOpt->distance(*selectedTarget) <= player.getViewRadius();
        buffer += "Selected cell effects: " +
                  (visible || cell.isDiscovered() ? describeStatuses(cell) : "Unknown") + "\n";
    }

    buffer += "\nAbilities:\n";
    if (player.getAbilityCount() == 0) {
        buffer += "  None\n";
    } else {
        for (std::size_t index = 0; index < player.getAbilityCount(); ++index) {
            const IAbility* ability = player.getAbility(index);
            if (!ability) continue;

            buffer += "  " + std::to_string(index + 1) + ". " +
                      std::string(ability->getName()) + " (" +
                      std::to_string(ability->getEnergyCost()) + " energy)";
            if (player.getEnergy() < ability->getEnergyCost()) {
                buffer += " - not enough energy";
            }
            buffer += "\n";
        }
    }

    if (movesLeft >= 0) {
        buffer += "\nMoves left: " + std::to_string(movesLeft) + "/" + std::to_string(maxMoves);
        if (selectedTarget) {
            buffer += "\nW/A/S/D - select cell; Enter - confirm; Esc - cancel\n";
        } else {
            buffer += "\nW/A/S/D - move/interact; Space - end turn\n";
            if (player.getAbilityCount() > 0) {
                buffer += "1-9 - use ability by number\n";
            }
        }
    }
    if (!message.empty()) buffer += "\n" + message + "\n";
    clearConsole();

    const HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(output, &info)) {
        std::cout << buffer << std::flush;
        return;
    }
    const WORD normalColor = info.wAttributes;
    const WORD fogColor = static_cast<WORD>((normalColor & ~0x000F) | FOREGROUND_INTENSITY);
    const WORD targetColor = BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN |
                             FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    std::size_t printed = 0;
    for (const auto& [begin, end, isTarget] : colorRanges) {
        std::cout.write(buffer.data() + printed, begin - printed);
        std::cout << std::flush;
        SetConsoleTextAttribute(output, isTarget ? targetColor : fogColor);
        std::cout.write(buffer.data() + begin, end - begin);
        std::cout << std::flush;
        SetConsoleTextAttribute(output, normalColor);
        printed = end;
    }
    std::cout.write(buffer.data() + printed, buffer.size() - printed);
    std::cout << std::flush;
}
