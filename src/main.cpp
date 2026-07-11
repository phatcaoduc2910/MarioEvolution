#include "core/Game.h"

int main() {
    Game game;

    if (!game.start()) {
        return 1;
    }

    game.gameLoop();

    return 0;
}
