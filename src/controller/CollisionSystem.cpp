#include "controller/CollisionSystem.h"

#include "model/Actor.h"
#include "model/GameObject.h"
#include "model/StaticObject.h"
#include "model/World.h"

/**
 * Kiểm tra có va chạm object hay không.
 */
bool CollisionSystem::check(
    const Actor& actor,
    const GameObject& object
) const {
    const Rectangle a = actor.getBounds();
    const Rectangle b = object.getBounds();

    return a.x < b.x + b.width &&
           a.x + a.width > b.x &&
           a.y < b.y + b.height &&
           a.y + a.height > b.y;
}

/**
 * Chia trường hợp để giải quyết va chạm.
 */
void CollisionSystem::resolve(World& world) {
    Player& player = world.getPlayer();

    for (const auto& object : world.getObjects()) {
        if (object->isSolid() && check(player, *object)) {
            object->onCollision(player);
        }
    }

    for (const auto& actor : world.getActors()) {
        if (!actor->isAlive()) {
            continue;
        }

        for (const auto& object : world.getObjects()) {
            if (object->isSolid() && check(*actor, *object)) {
                object->onCollision(*actor);
            }
        }
    }
}
