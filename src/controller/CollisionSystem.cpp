#include "controller/CollisionSystem.h"

#include "model/Actor.h"
#include "model/Brick.h"
#include "model/Flag.h"
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

void CollisionSystem::resolve(World& world) {
    Player& player = world.getPlayer();

    // A2/A6: Xử lý Flag trước để không xem Flag như brick hoặc vật cản rắn.
    for (const auto& object : world.getObjects()) {
        if (auto* flag = dynamic_cast<Flag*>(object.get())) {
            if (!flag->isCaptured() && check(player, *flag)) {
                player.captureFlag(*flag);
                // A6: Gọi World::markLevelComplete() tại đây sau khi B bổ sung contract.
            }
            continue;
        }

        if (!object->isSolid() || !check(player, *object)) {
            continue;
        }

        // A2: Chỉ Brick mới phản ứng khi bị đập từ phía dưới.
        if (auto* brick = dynamic_cast<Brick*>(object.get());
            brick != nullptr && hitsFromBelow(player, *brick)) {
            if (auto* specialBrick = dynamic_cast<SpecialBrick*>(brick)) {
                std::unique_ptr<Item> item = specialBrick->releaseItem();
                if (item != nullptr) {
                    world.addItem(std::move(item));
                }
            } else {
                brick->hitBy(player);
            }
        }

        object->onCollision(player);
    }

    // A3: Trạng thái collected ngăn áp dụng hiệu ứng và cộng điểm nhiều lần.
    for (const auto& item : world.getItems()) {
        if (item->isCollected() || !check(player, *item)) {
            continue;
        }

        if (const auto* coin = dynamic_cast<const Coin*>(item.get())) {
            world.addScore(coin->getValue());
        }
        player.collect(*item);
    }
    //Xử lý các actor với brick
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
