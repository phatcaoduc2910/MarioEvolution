#include "controller/CollisionSystem.h"

#include "model/Actor.h"
#include "model/Brick.h"
#include "model/Enemy.h"
#include "model/Flag.h"
#include "model/GameObject.h"
#include "model/Item.h"
#include "model/StaticObject.h"
#include "model/World.h"

#include <memory>
#include <utility>

namespace {
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

void CollisionSystem::update(World& world, double dtSeconds) {
    Player& player = world.getPlayer();
    if (player.isAlive()) {
        stepActor(player, world, dtSeconds);
    }

    for (const auto& actor : world.getActors()) {
        if (actor->isAlive()) {
            stepActor(*actor, world, dtSeconds);
        }
    }

    resolveInteractions(world);
}

// Bước cố định 11 ms cho tốc độ rơi tối đa ra 7.92 px, nhỏ hơn ô 32 px nên
// không cần quét đường đi: actor không thể nhảy qua block giữa hai frame.
void CollisionSystem::stepActor(
    Actor& actor,
    World& world,
    double dtSeconds
) const {
    actor.moveX(dtSeconds);
    const bool blockedByWall = resolveX(actor, world);

    actor.moveY(dtSeconds);
    resolveY(actor, world);

    // A5: Enemy chỉ lật hướng khi trục X thật sự bị chặn, không đoán theo độ lún.
    if (blockedByWall) {
        if (auto* enemy = dynamic_cast<Enemy*>(&actor)) {
            enemy->reverseDirection();
        }
    }
}

bool CollisionSystem::resolveX(Actor& actor, const World& world) const {
    // Hướng đi đọc một lần: cú tách đầu tiên xoá velocityX của các cú sau.
    const double movingX = actor.getVelocityX();
    if (movingX == 0.0) {
        return false;
    }

    bool blocked = false;
    for (const auto& object : world.getObjects()) {
        if (!object->isSolid() || !check(actor, *object)) {
            continue;
        }

        const Rectangle actorBounds = actor.getBounds();
        const Rectangle objectBounds = object->getBounds();

        actor.placeBesideWall(
            (movingX > 0.0)
                ? objectBounds.x - actorBounds.width
                : objectBounds.x + objectBounds.width);
        blocked = true;
    }

    return blocked;
}

void CollisionSystem::resolveY(Actor& actor, World& world) const {
    const double movingY = actor.getVelocityY();
    if (movingY == 0.0) {
        return;
    }

    for (const auto& object : world.getObjects()) {
        if (!object->isSolid() || !check(actor, *object)) {
            continue;
        }

        const Rectangle actorBounds = actor.getBounds();
        const Rectangle objectBounds = object->getBounds();

        if (movingY > 0.0) {
            actor.placeOnGround(objectBounds.y - actorBounds.height);
            continue;
        }

        actor.placeUnderCeiling(objectBounds.y + objectBounds.height);

        // A2: Đập gạch chính là cú chạm trần, không phải phép so tâm hai hộp.
        if (auto* player = dynamic_cast<Player*>(&actor)) {
            hitBrickFromBelow(*player, *object, world);
        }
    }
}

void CollisionSystem::hitBrickFromBelow(
    Player& player,
    StaticObject& object,
    World& world
) const {
    auto* brick = dynamic_cast<Brick*>(&object);
    if (brick == nullptr) {
        return;
    }

    if (auto* specialBrick = dynamic_cast<SpecialBrick*>(brick)) {
        std::unique_ptr<Item> item = specialBrick->releaseItem();
        if (item != nullptr) {
            world.addItem(std::move(item));
        }
        return;
    }

    brick->hitBy(player);
}

void CollisionSystem::resolveInteractions(World& world) const {
    Player& player = world.getPlayer();

    // A2/A6: Flag là vùng kích hoạt nên chỉ xét ở đây, không nằm trong resolve trục.
    for (const auto& object : world.getObjects()) {
        auto* flag = dynamic_cast<Flag*>(object.get());
        if (flag == nullptr || flag->isCaptured() || !check(player, *flag)) {
            continue;
        }

        player.captureFlag(*flag);
        world.markLevelComplete();
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
}
