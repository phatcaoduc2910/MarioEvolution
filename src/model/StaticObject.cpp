#include "model/StaticObject.h"

#include "model/Actor.h"

StaticObject::StaticObject(double x, double y, int width, int height)
    : GameObject(x, y, width, height),
      solid(true) {}

bool StaticObject::isSolid() const {
    return solid;
}

void StaticObject::onCollision(Actor& actor) {
    if (solid) {
        actor.resolveCollision(*this);
    }
}
