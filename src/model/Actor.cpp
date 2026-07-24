#include "model/Actor.h"

#include <algorithm>

namespace {
constexpr double kGravity = 0.45;
constexpr double kMaxFallSpeed = 12.0;

bool intersects(const Rectangle& a, const Rectangle& b) {
    return a.x < b.x + b.width &&
           a.x + a.width > b.x &&
           a.y < b.y + b.height &&
           a.y + a.height > b.y;
}
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

void Actor::move() {
    x += velocityX;
    y += velocityY;

    if (velocityX < 0.0) {
        direction = Direction::Left;
    } else if (velocityX > 0.0) {
        direction = Direction::Right;
    }
}

void Actor::applyGravity() {
    onGround = false;
    velocityY = std::min(velocityY + kGravity, kMaxFallSpeed);
}

void Actor::resolveCollision(GameObject& object) {
    const Rectangle self = getBounds();
    const Rectangle other = object.getBounds();

    if (!intersects(self, other)) {
        return;
    }

    const double selfCenterX = self.x + self.width / 2.0;
    const double selfCenterY = self.y + self.height / 2.0;
    const double otherCenterX = other.x + other.width / 2.0;
    const double otherCenterY = other.y + other.height / 2.0;

    const double overlapLeft = self.x + self.width - other.x;
    const double overlapRight = other.x + other.width - self.x;
    const double overlapTop = self.y + self.height - other.y;
    const double overlapBottom = other.y + other.height - self.y;

    const double overlapX = std::min(overlapLeft, overlapRight);
    const double overlapY = std::min(overlapTop, overlapBottom);

    // Tách theo trục lún ít hơn để actor không bị đẩy chéo qua vật cản.
    if (overlapX < overlapY) {
        x += (selfCenterX < otherCenterX) ? -overlapX : overlapX;
        velocityX = 0.0;
    } else {
        if (selfCenterY < otherCenterY) {
            y -= overlapY;
            onGround = true;
        } else {
            y += overlapY;
        }

        velocityY = 0.0;
    }
}
