#include "model/Actor.h"

#include <algorithm>

namespace {
constexpr double kGravity = 0.45;
constexpr double kMaxFallSpeed = 12.0;

/**
 * Checks whether two axis-aligned rectangles overlap.
 *
 * @param a First rectangle.
 * @param b Second rectangle.
 * @return true when the rectangles intersect; false otherwise.
 */
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
      alive(true) {}

bool Actor::isAlive() const {
    return alive;
}

Direction Actor::getDirection() const {
    return direction;
}

/**
 * Applies the current velocity to the actor position.
 *
 * The horizontal velocity also updates the facing direction so render and
 * gameplay systems can know whether the actor is moving left or right.
 */
void Actor::move() {
    x += velocityX;
    y += velocityY;

    if (velocityX < 0.0) {
        direction = Direction::Left;
    } else if (velocityX > 0.0) {
        direction = Direction::Right;
    }
}

/**
 * Applies vertical gravity to the actor.
 *
 * Fall speed is capped to avoid very large per-frame movement that could make
 * collision resolution unstable.
 */
void Actor::applyGravity() {
    velocityY = std::min(velocityY + kGravity, kMaxFallSpeed);
}

/**
 * Resolves collision against a solid game object using AABB overlap.
 *
 * This method only performs generic physical separation. Game-specific
 * decisions such as item pickup, enemy damage, score, or flag capture should
 * stay in CollisionSystem or the relevant gameplay class.
 *
 * @param object Object to separate this actor from.
 */
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

    if (overlapX < overlapY) {
        x += (selfCenterX < otherCenterX) ? -overlapX : overlapX;
        velocityX = 0.0;
    } else {
        y += (selfCenterY < otherCenterY) ? -overlapY : overlapY;
        velocityY = 0.0;
    }
}
