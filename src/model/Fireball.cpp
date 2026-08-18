#include "model/Fireball.h"

namespace {
    constexpr double kFireballSpeedPixelsPerSecond = 420.0;
    constexpr int kFireballSize = 16;
}

Fireball::Fireball(double x, double y, Direction dir)
    :  Actor(x, y, kFireballSize, kFireballSize) {
        direction = dir;
        velocityX = (dir == Direction::Left)
                        ? -kFireballSpeedPixelsPerSecond
                        : kFireballSpeedPixelsPerSecond;
        velocityY = 0.0;
    }

void Fireball::update(double dtSeconds) {
    if (!alive) { return; }
    applyGravity(dtSeconds);
    // TODO: Fireball chưa nằm trong World nên tự đi, chưa qua CollisionSystem - model/World.
    moveX(dtSeconds);
    moveY(dtSeconds);
}

void Fireball::destroy() {
    alive = false;
    velocityX = 0.0;
    velocityY = 0.0;
}
