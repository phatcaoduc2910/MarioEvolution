#include "model/Actor.h"

#include <algorithm>

namespace {
constexpr double kGravityPixelsPerSecondSquared = 1620.0;
constexpr double kMaxFallSpeedPixelsPerSecond = 720.0;

std::uint32_t nextActorId() {
    static std::uint32_t counter = 0;
    return ++counter;
}
}

Actor::Actor(double x, double y, int width, int height)
    : GameObject(x, y, width, height),
      id(nextActorId()),
      velocityX(0.0),
      velocityY(0.0),
      direction(Direction::Right),
      alive(true),
      onGround(false),
      landingImpactSpeed(0.0),
      groundedBeforeStep(false) {}


std::uint32_t Actor::getId() const {
    return id;
}


bool Actor::isAlive() const {
    return alive;
}


bool Actor::isRemovable() const {
    return !isAlive();
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


double Actor::getLandingImpactSpeed() const {
    return landingImpactSpeed;
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
    groundedBeforeStep = onGround;
    onGround = false;
}

void Actor::placeBesideWall(double colliderX) {
    x += colliderX - getBounds().x;
    velocityX = 0.0;
}

void Actor::placeOnGround(double colliderY) {
    y += colliderY - getBounds().y;
    if (!groundedBeforeStep) {
        landingImpactSpeed = velocityY;
    }
    velocityY = 0.0;
    onGround = true;
}

void Actor::placeUnderCeiling(double colliderY) {
    y += colliderY - getBounds().y;
    velocityY = 0.0;
}
