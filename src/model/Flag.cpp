#include "model/Flag.h"
#include "model/Player.h"
#include <SDL2/SDL.h>

Flag::Flag(double x, double y)
    : StaticObject(x, y, kPoleWidth, kPoleHeight),
      captured(false) {
    solid = false;
}

bool Flag::isCaptured() const {
    return captured;
}

void Flag::onCapture(Player& player) {
    if (captured || !player.isAlive() || player.getState() == PlayerState::Dead) {
        return;
    }

    captured = true;
    showGameOverMessage();
}

// Ghi nhận hoàn thành màn chơi.
void Flag::showGameOverMessage() {
    SDL_Log("Level completed!");
}
