#include "model/Fireball.h"

namespace {
constexpr double kFireballSpeedPixelsPerSecond = 420.0;
constexpr double kBounceVelocityPixelsPerSecond = -320.0;
}

Fireball::Fireball(double x, double y, Direction dir)
    : Actor(x, y, kSize, kSize), hasBounced(false) {
    direction = dir;
    velocityX = (dir == Direction::Left)
                    ? -kFireballSpeedPixelsPerSecond
                    : kFireballSpeedPixelsPerSecond;
    velocityY = 0.0;
}

void Fireball::update(double dtSeconds) {
    if (!alive) { return; }
    applyGravity(dtSeconds);
}

void Fireball::bounce() {
    if (hasBounced) {
        destroy();
        return;
    }

    hasBounced = true;
    velocityY = kBounceVelocityPixelsPerSecond;
    onGround = false;
}

void Fireball::destroy() {
    alive = false;
    velocityX = 0.0;
    velocityY = 0.0;
}
