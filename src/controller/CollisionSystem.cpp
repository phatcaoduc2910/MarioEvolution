#include "controller/CollisionSystem.h"

#include "model/Actor.h"
#include "model/Brick.h"
#include "model/Enemy.h"
#include "model/Flag.h"
#include "model/GameObject.h"
#include "model/Item.h"
#include "model/StaticObject.h"
#include "model/World.h"

#include <algorithm>
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

bool isStomp(const Player& player, const Enemy& enemy) {
    if (player.getVelocityY() <= 0.0) {
        return false;
    }

    const Rectangle playerBounds = player.getBounds();
    const Rectangle enemyBounds = enemy.getBounds();
    const double playerBottom = playerBounds.y + playerBounds.height;
    const double verticalPenetration = playerBottom - enemyBounds.y;

    return playerBounds.y < enemyBounds.y &&
           verticalPenetration >= 0.0 &&
           verticalPenetration <= 14.0;
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
                // A6: Phát tín hiệu hoàn thành level sau khi Player chiếm cờ.
                world.markLevelComplete();
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

    // A4: Va chạm Enemy là xử lý gameplay, không phải resolve vật cản rắn.
    for (const auto& actor : world.getActors()) {
        auto* enemy = dynamic_cast<Enemy*>(actor.get());
        if (enemy == nullptr || !enemy->isAlive() ||
            !check(player, *enemy)) {
            continue;
        }

        if (isStomp(player, *enemy)) {
            enemy->die();
            player.bounceAfterStomp();
            world.addScore(100);
        } else {
            enemy->damagePlayer(player);
        }
    }

    // A5: Enemy đổi hướng sau khi resolver tách Enemy khỏi tường.
    for (const auto& actor : world.getActors()) {
        if (!actor->isAlive()) {
            continue;
        }

        for (const auto& object : world.getObjects()) {
            if (object->isSolid() && check(*actor, *object)) {
                const Rectangle actorBounds = actor->getBounds();
                const Rectangle objectBounds = object->getBounds();
                const double overlapX = std::min(
                    actorBounds.x + actorBounds.width - objectBounds.x,
                    objectBounds.x + objectBounds.width - actorBounds.x);
                const double overlapY = std::min(
                    actorBounds.y + actorBounds.height - objectBounds.y,
                    objectBounds.y + objectBounds.height - actorBounds.y);
                const bool horizontalHit = overlapX < overlapY;

                object->onCollision(*actor);

                if (horizontalHit) {
                    if (auto* enemy = dynamic_cast<Enemy*>(actor.get())) {
                        enemy->reverseDirection();
                    }
                }
            }
        }
    }
}
