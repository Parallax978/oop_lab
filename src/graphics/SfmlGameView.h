#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "GraphicsRenderer.h"

struct GameViewClosed {};

namespace GameInput {
    inline constexpr int Enter = 13;
    inline constexpr int Escape = 27;
    inline constexpr int Space = 32;
}

class SfmlGameView {
public:
    SfmlGameView();
    int readKey();
    void clearInput();
    void pause(unsigned milliseconds);
    void showGameResult(bool won);
    void renderRankUpMenu(const Robot& player, int rank,
        const std::vector<std::unique_ptr<IAbility>>& newAbilities);
    void render(const Field& field, const Robot& player, int turnNumber,
        int movesLeft, int maxMoves, const std::string& message,
        std::optional<Point> selectedTarget, bool showStatusCells);
    std::unique_ptr<IAbility> selectStartingAbility(int energy);

private:
    std::optional<int> handleEvent(const sf::Event& event);
    void resize(sf::Vector2u size);
    void redraw();
    void present();
    void text(const std::string& value, sf::Vector2f position, unsigned size = 20,
              sf::Color color = sf::Color(220, 230, 240));
    void menu(std::string title, std::vector<std::string> options,
              std::string footer = "Choose a number (no Enter needed). Close the window to exit.");

    sf::RenderWindow window_;
    sf::RenderTexture canvas_;
    sf::Font font_;
    GraphicsRenderer renderer_;
    std::function<void()> draw_;
};
