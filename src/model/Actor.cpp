#include "model/Actor.h"

#include <algorithm>

namespace {
constexpr double kGravityPixelsPerSecondSquared = 1620.0;
constexpr double kMaxFallSpeedPixelsPerSecond = 720.0;
}

Actor::Actor(double x, double y, int width, int height)
    : GameObject(x, y, width, height),
      velocityX(0.0),
      velocityY(0.0),
      direction(Direction::Right),
      alive(true),
      onGround(false) {}


bool Actor::isAlive() const {
    return alive;
}


bool Actor::isOnGround() const {
    return onGround;
}


Direction Actor::getDirection() const {
    return direction;
}


double Actor::getVelocityX() const {
    return velocityX;
}


double Actor::getVelocityY() const {
    return velocityY;
}

void Actor::applyGravity(double dtSeconds) {
    velocityY = std::min(
        velocityY + kGravityPixelsPerSecondSquared * dtSeconds,
        kMaxFallSpeedPixelsPerSecond);
}

void Actor::moveX(double dtSeconds) {
    x += velocityX * dtSeconds;

    if (velocityX < 0.0) {
        direction = Direction::Left;
    } else if (velocityX > 0.0) {
        direction = Direction::Right;
    }
}

void Actor::moveY(double dtSeconds) {
    // Rời chỗ theo trục dọc là mất tiếp đất; chỉ resolve trục Y đặt lại onGround.
    y += velocityY * dtSeconds;
    onGround = false;
}

void Actor::placeBesideWall(double colliderX) {
    x += colliderX - getBounds().x;
    velocityX = 0.0;
}

void Actor::placeOnGround(double colliderY) {
    y += colliderY - getBounds().y;
    velocityY = 0.0;
    onGround = true;
}

void Actor::placeUnderCeiling(double colliderY) {
    y += colliderY - getBounds().y;
    velocityY = 0.0;
}
