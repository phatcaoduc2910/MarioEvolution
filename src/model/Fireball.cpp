#include "model/Fireball.h"

namespace {
    constexpr double kFireballSpeed = 7.0;
    constexpr int kFireballSize = 16;
}

Fireball::Fireball(double x, double y, Direction dir)
    :  Actor(x, y, kFireballSize, kFireballSize) {
        direction = dir;
        velocityX = (dir == Direction::Left) ? -kFireballSpeed : kFireballSpeed;
        velocityY = 0.0;
    }

void Fireball::update() {
    if (!alive) { return; }
    applyGravity();
    move();
}

void Fireball::destroy() {
    alive = false;
    velocityX = 0.0;
    velocityY = 0.0;
}
