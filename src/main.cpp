#include <exception>
#include <iostream>
#include <utility>
#include "GameManager.h"
#include "SfmlGameView.h"
#include "StartingField.h"

int main() {
    try {
        SfmlGameView view;
        auto start = createStartingField();
        start.player->addAbility(view.selectStartingAbility(start.player->getEnergy()));
        GameManager game(std::move(start.field), start.player, view);
        game.run();
    } catch (const GameViewClosed&) {
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Cannot start graphical game: " << error.what() << '\n';
        return 1;
    }
}
