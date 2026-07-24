#include "model/Flag.h"

#include "model/Player.h"

#include <SDL2/SDL.h>

Flag::Flag(double x, double y)
    : StaticObject(x, y, 32, 160),
      captured(false) {}


bool Flag::isCaptured() const {
    return captured;
}

void Flag::onCapture(Player& player) {
    if (captured || !player.isAlive() || player.getState() == PlayerState::Dead) {
        return;
    }

    captured = true;
    solid = false;
    showGameOverMessage();
}

void Flag::showGameOverMessage() {
    SDL_Log("Level completed!");
}
