#include "model/Player.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace {
bool nearlyEqual(double a, double b) {
    return std::abs(a - b) < 1e-9;
}
}

int main() {
    Player player(0.0, 0.0);

    player.setMoveDirection(1);
    player.update(0.1);
    assert(nearlyEqual(player.getVelocityX(), 120.0));

    player.update(0.1);
    assert(nearlyEqual(player.getVelocityX(), 240.0));

    const double releaseX = player.getX();
    player.setMoveDirection(0);
    player.update(0.1);
    assert(nearlyEqual(player.getVelocityX(), 160.0));
    assert(player.getX() > releaseX);

    player.update(0.2);
    assert(nearlyEqual(player.getVelocityX(), 0.0));

    std::cout << "Player inertia validation passed\n";
    return 0;
}
