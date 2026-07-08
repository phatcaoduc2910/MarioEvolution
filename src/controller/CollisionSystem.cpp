#include "controller/CollisionSystem.h"

#include "model/Actor.h"
#include "model/GameObject.h"
#include "model/World.h"

bool CollisionSystem::check(const Actor& actor, const GameObject& object) const {
    (void)actor;
    (void)object;
    return false;
}

void CollisionSystem::resolve(World& world) {
    (void)world;
}
