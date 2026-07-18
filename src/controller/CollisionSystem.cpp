#include "controller/CollisionSystem.h"

#include "model/Actor.h"
#include "model/Brick.h"
#include "model/GameObject.h"
#include "model/Item.h"
#include "model/StaticObject.h"
#include "model/World.h"

#include <memory>
#include <utility>

namespace {
bool hitsFromBelow(const Actor& actor, const GameObject& object) {
    const Rectangle actorBounds = actor.getBounds();
    const Rectangle objectBounds = object.getBounds();
    const double actorCenterY = actorBounds.y + actorBounds.height / 2.0;
    const double objectCenterY = objectBounds.y + objectBounds.height / 2.0;

    return actor.getVelocityY() < 0.0 && actorCenterY > objectCenterY;
}
}

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
            if (hitsFromBelow(player, *object)) {
                if (auto* specialBrick =
                        dynamic_cast<SpecialBrick*>(object.get())) {
                    std::unique_ptr<Item> item = specialBrick->releaseItem();
                    if (item != nullptr) {
                        world.addItem(std::move(item));
                    }
                } else if (auto* brick = dynamic_cast<Brick*>(object.get())) {
                    brick->hitBy(player);
                }
            }

            object->onCollision(player);
        }
    }

    for (const auto& item : world.getItems()) {
        if (item->isCollected() || !check(player, *item)) {
            continue;
        }

        if (const auto* coin = dynamic_cast<const Coin*>(item.get())) {
            world.addScore(coin->getValue());
        }
        player.collect(*item);
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
