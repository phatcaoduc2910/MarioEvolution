#include "model/Flag.h"

#include "model/Player.h"

Flag::Flag(double x, double y)
    : StaticObject(x, y, 32, 160),
      captured(false) {}

bool Flag::isCaptured() const {
    return captured;
}

void Flag::onCapture(Player& player) {
    (void)player;
}

void Flag::showGameOverMessage() {}
