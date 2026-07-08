#include "model/Actor.h"

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

void Actor::move() {}

void Actor::applyGravity() {}

void Actor::resolveCollision(GameObject& object) {
    (void)object;
}
