#include "SfmlGameView.h"

#include <filesystem>
#include <stdexcept>
#include "AreaAttack.h"
#include "RangedAttack.h"
#include "Healing.h"
#include "Teleportation.h"

namespace {
std::string wrap(const std::string& input, std::size_t width) {
    std::string result;
    std::size_t column = 0;
    for (char character : input) {
        if (character == '\n') column = 0;
        else if (column >= width && character == ' ') {
            result += '\n';
            column = 0;
            continue;
        } else ++column;
        result += character;
    }
    return result;
}
}

SfmlGameView::SfmlGameView()
    : window_(sf::VideoMode({1100, 760}), "RoboGame"), canvas_({1100, 760}) {
    window_.setFramerateLimit(60);
    window_.setKeyRepeatEnabled(false);
    bool loaded = false;
    for (const auto& path : {"assets/fonts/DejaVuSans.ttf", "C:/Windows/Fonts/segoeui.ttf",
                             "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                             "/System/Library/Fonts/Supplemental/Arial.ttf"}) {
        if (std::filesystem::exists(path) && font_.openFromFile(path)) {
            loaded = true;
            break;
        }
    }
    if (!loaded) throw std::runtime_error("Font not found. Add assets/fonts/DejaVuSans.ttf.");
    resize(window_.getSize());
}

void SfmlGameView::resize(sf::Vector2u size) {
    if (size.x == 0 || size.y == 0) return;
    sf::View view(sf::FloatRect({0.f, 0.f}, {1100.f, 760.f}));
    const float aspect = static_cast<float>(size.x) / static_cast<float>(size.y);
    const float desired = 1100.f / 760.f;
    if (aspect > desired) {
        const float width = desired / aspect;
        view.setViewport(sf::FloatRect({(1.f - width) / 2.f, 0.f}, {width, 1.f}));
    } else {
        const float height = aspect / desired;
        view.setViewport(sf::FloatRect({0.f, (1.f - height) / 2.f}, {1.f, height}));
    }
    window_.setView(view);
}

std::optional<int> SfmlGameView::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        window_.close();
        throw GameViewClosed{};
    }
    if (const auto* resized = event.getIf<sf::Event::Resized>()) resize(resized->size);
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        using Scan = sf::Keyboard::Scancode;
        switch (key->scancode) {
        case Scan::W: return 'W';
        case Scan::A: return 'A';
        case Scan::S: return 'S';
        case Scan::D: return 'D';
        case Scan::Q: return 'Q';
        default: break;
        }
        using Key = sf::Keyboard::Key;
        if (key->code >= Key::Num1 && key->code <= Key::Num9)
            return '1' + static_cast<int>(key->code) - static_cast<int>(Key::Num1);
        if (key->code >= Key::Numpad1 && key->code <= Key::Numpad9)
            return '1' + static_cast<int>(key->code) - static_cast<int>(Key::Numpad1);
        if (key->code == Key::Enter) return GameInput::Enter;
        if (key->code == Key::Escape) return GameInput::Escape;
        if (key->code == Key::Space) return GameInput::Space;
    }
    return std::nullopt;
}

int SfmlGameView::readKey() {
    while (window_.isOpen()) {
        while (const auto event = window_.pollEvent()) {
            if (auto key = handleEvent(*event)) return *key;
        }
        redraw();
    }
    throw GameViewClosed{};
}

void SfmlGameView::clearInput() {
    while (const auto event = window_.pollEvent()) handleEvent(*event);
}

void SfmlGameView::pause(unsigned milliseconds) {
    sf::Clock clock;
    while (clock.getElapsedTime().asMilliseconds() < static_cast<std::int64_t>(milliseconds)) {
        clearInput();
        redraw();
    }
}

void SfmlGameView::redraw() {
    if (draw_) draw_();
}

void SfmlGameView::present() {
    canvas_.display();
    window_.clear(sf::Color(12, 17, 25));
    window_.draw(sf::Sprite(canvas_.getTexture()));
    window_.display();
}

void SfmlGameView::text(const std::string& value, sf::Vector2f position,
                        unsigned size, sf::Color color) {
    sf::Text label(font_, value, size);
    label.setPosition(position);
    label.setFillColor(color);
    canvas_.draw(label);
}

void SfmlGameView::render(const Field& field, const Robot& player, int turnNumber,
                          int movesLeft, int maxMoves, const std::string& message,
                          std::optional<Point> selectedTarget, bool showStatusCells) {
    draw_ = [this, &field, &player, turnNumber, movesLeft, maxMoves, message,
             selectedTarget, showStatusCells] {
        canvas_.clear(sf::Color(17, 23, 34));
        sf::View board(sf::FloatRect({0.f, 0.f}, {720.f, 620.f}));
        board.setViewport(sf::FloatRect({24.f / 1100.f, 95.f / 760.f},
                                       {720.f / 1100.f, 620.f / 760.f}));
        canvas_.setView(board);
        renderer_.render(canvas_, field, player, showStatusCells, selectedTarget);
        canvas_.setView(canvas_.getDefaultView());
        text("ROBOGAME", {40.f, 22.f}, 30, sf::Color(65, 205, 180));
        text("Turn " + std::to_string(turnNumber), {790.f, 30.f}, 24);
        std::string info = "HP: " + std::to_string(player.getHp()) + "/" +
            std::to_string(player.getMaxHp()) + "\nEnergy: " +
            std::to_string(player.getEnergy()) + "/" + std::to_string(player.getMaxEnergy()) +
            "\nRank: " + std::to_string(player.getRank()) + "  XP: " +
            std::to_string(player.getExp()) + "/" + std::to_string(player.getExpToNextRank());
        if (movesLeft >= 0) info += "\nMoves: " + std::to_string(movesLeft) + "/" + std::to_string(maxMoves);
        text(info, {790.f, 95.f});
        text("ABILITIES", {790.f, 225.f}, 20, sf::Color(65, 205, 180));
        for (std::size_t i = 0; i < player.getAbilityCount(); ++i) {
            const auto* ability = player.getAbility(i);
            text(std::to_string(i + 1) + ". " + std::string(ability->getName()) +
                 " [" + std::to_string(ability->getEnergyCost()) + "]",
                 {790.f, 258.f + static_cast<float>(i) * 29.f}, 18,
                 player.getEnergy() >= ability->getEnergyCost() ? sf::Color(220, 230, 240)
                                                               : sf::Color(135, 145, 160));
        }
        text("STATUSES", {790.f, 400.f}, 20, sf::Color(65, 205, 180));
        std::string statuses;
        for (std::size_t i = 0; i < player.getStatusCount(); ++i) {
            if (!statuses.empty()) statuses += '\n';
            statuses += wrap(player.getStatus(i)->getDescription(), 24);
        }
        text(statuses.empty() ? "None" : statuses, {790.f, 434.f}, 17);
        text(selectedTarget ? "W/A/S/D - select target\nEnter - confirm\nEsc - cancel"
                            : "W/A/S/D - move / interact\n1-9 - ability\nSpace - end turn",
             {790.f, 615.f}, 17);
        text(showStatusCells ? "Q - hide status cells" : "Q - show status cells",
             {790.f, 690.f}, 17);
        text(wrap(message, 100), {40.f, 718.f}, 17, sf::Color(245, 200, 110));
        present();
    };
    redraw();
}

void SfmlGameView::menu(std::string title, std::vector<std::string> options, std::string footer) {
    draw_ = [this, title = std::move(title), options = std::move(options), footer = std::move(footer)] {
        canvas_.setView(canvas_.getDefaultView());
        canvas_.clear(sf::Color(17, 23, 34));
        text(title, {90.f, 100.f}, 32, sf::Color(65, 205, 180));
        for (std::size_t i = 0; i < options.size(); ++i)
            text(options[i], {95.f, 210.f + static_cast<float>(i) * 60.f}, 23);
        text(footer, {95.f, 660.f}, 19);
        present();
    };
    redraw();
}

void SfmlGameView::renderRankUpMenu(const Robot& player, int rank,
    const std::vector<std::unique_ptr<IAbility>>& newAbilities) {
    std::vector<std::string> options;
    for (std::size_t i = 0; i < player.getAbilityCount(); ++i)
        options.push_back(std::to_string(options.size() + 1) + ". Upgrade " +
                          std::string(player.getAbility(i)->getName()));
    for (const auto& ability : newAbilities)
        options.push_back(std::to_string(options.size() + 1) + ". Learn " +
                          std::string(ability->getName()));
    menu("Rank " + std::to_string(rank) + " reached - choose one reward", std::move(options));
}

void SfmlGameView::showGameResult(bool won) {
    menu(won ? "VICTORY" : "DEFEAT", {}, "Press Enter to close the game.");
}

std::unique_ptr<IAbility> SfmlGameView::selectStartingAbility(int energy) {
    std::vector<std::unique_ptr<IAbility>> abilities;
    abilities.push_back(std::make_unique<AreaAttack>());
    abilities.push_back(std::make_unique<RangedAttack>());
    abilities.push_back(std::make_unique<Healing>());
    abilities.push_back(std::make_unique<Teleportation>());
    std::vector<std::string> options;
    for (const auto& ability : abilities)
        options.push_back(std::to_string(options.size() + 1) + ". " +
            std::string(ability->getName()) + " - " + std::to_string(ability->getEnergyCost()) + " energy");
    menu("Choose your starting ability", std::move(options));
    while (true) {
        const int key = readKey();
        if (key == GameInput::Escape) throw GameViewClosed{};
        if (key < '1' || key > '4') continue;
        const auto index = static_cast<std::size_t>(key - '1');
        if (abilities[index]->getEnergyCost() <= energy) return std::move(abilities[index]);
    }
}
