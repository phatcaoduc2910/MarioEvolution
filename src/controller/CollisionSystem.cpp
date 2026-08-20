#include "controller/CollisionSystem.h"

#include "model/Actor.h"
#include "model/Boss.h"
#include "model/Brick.h"
#include "model/Enemy.h"
#include "model/Fireball.h"
#include "model/Flag.h"
#include "model/GameObject.h"
#include "model/Item.h"
#include "model/StaticObject.h"
#include "model/World.h"

#include <memory>
#include <utility>

namespace {
constexpr int kEdgeProbeWidth = 1;
constexpr int kEdgeProbeHeight = 2;
constexpr int kEnemyDefeatScore = 100;

bool overlaps(const Rectangle& a, const Rectangle& b) {
    return a.x < b.x + b.width &&
           a.x + a.width > b.x &&
           a.y < b.y + b.height &&
           a.y + a.height > b.y;
}

// Kiểm tra xem có phải giẫm lên enemy hay không  
bool isStomp(const Player& player, const Enemy& enemy, double dtSeconds) {
    const double fallSpeed = player.getVelocityY();
    if (fallSpeed <= 0.0) {
        return false;
    }

    const Rectangle playerBounds = player.getBounds();
    const Rectangle enemyBounds = enemy.getBounds();
    const double previousPlayerBottom =
        playerBounds.y + playerBounds.height - fallSpeed * dtSeconds;

    return playerBounds.y < enemyBounds.y &&
           previousPlayerBottom <= enemyBounds.y;
}

void emitAtBottomCenter(World& world, VisualEventType type,
                        const GameObject& object, Direction direction) {
    const Rectangle bounds = object.getBounds();
    world.emitVisualEvent(
        type,
        bounds.x + bounds.width / 2.0,
        bounds.y + bounds.height,
        direction);
}

void emitAtCenter(World& world, VisualEventType type,
                  const GameObject& object, Direction direction) {
    const Rectangle bounds = object.getBounds();
    world.emitVisualEvent(
        type,
        bounds.x + bounds.width / 2.0,
        bounds.y + bounds.height / 2.0,
        direction);
}
}

// Xét collision chung cho GameObject và Actor
bool CollisionSystem::check(
    const Actor& actor,
    const GameObject& object
) const {
    return overlaps(actor.getBounds(), object.getBounds());
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

    resolveInteractions(world, dtSeconds);
}

void CollisionSystem::stepActor(
    Actor& actor,
    World& world,
    double dtSeconds
) const {
    actor.moveX(dtSeconds);
    const bool blockedByWall = resolveX(actor, world);

    // Nếu fireball chạm tường thì huỷ
    auto* fireball = dynamic_cast<Fireball*>(&actor);
    if (fireball != nullptr && blockedByWall) {
        emitAtCenter(world, VisualEventType::FireballImpact, *fireball,
                     fireball->getDirection());
        fireball->destroy();
        return;
    }

    actor.moveY(dtSeconds);
    resolveY(actor, world);

    if (fireball != nullptr) {
        return;
    }

    auto* enemy = dynamic_cast<Enemy*>(&actor);
    if (enemy == nullptr) {
        return;
    }

    if (blockedByWall) {
        enemy->reverseDirection();
        return;
    }

    if (enemy->shouldTurnAtEdge() && enemy->isOnGround() &&
        !hasGroundAhead(*enemy, world)) {
        enemy->reverseDirection();
    }
}

bool CollisionSystem::hasGroundAhead(
    const Actor& actor,
    const World& world
) const {
    const Rectangle bounds = actor.getBounds();
    const Rectangle probe{
        (actor.getDirection() == Direction::Left)
            ? bounds.x - kEdgeProbeWidth
            : bounds.x + bounds.width,
        bounds.y + bounds.height,
        kEdgeProbeWidth,
        kEdgeProbeHeight
    };

    for (const auto& object : world.getObjects()) {
        if (object->isSolid() && overlaps(probe, object->getBounds())) {
            return true;
        }
    }

    return false;
}

bool CollisionSystem::resolveX(Actor& actor, const World& world) const {
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

    bool bumped = false;

    for (const auto& object : world.getObjects()) {
        if (!object->isSolid() || !check(actor, *object)) {
            continue;
        }

        const Rectangle actorBounds = actor.getBounds();
        const Rectangle objectBounds = object->getBounds();

        if (movingY > 0.0) {
            actor.placeOnGround(objectBounds.y - actorBounds.height);
            //Cho fireball nảy khi chạm nền.
            auto* fireball = dynamic_cast<Fireball*>(&actor);
            if (fireball != nullptr) {
                fireball->bounce();
                return;
            }
            continue;
        }

        actor.placeUnderCeiling(objectBounds.y + objectBounds.height);
        // Chỉ xử lý brick khi đập ở dưới
        auto* player = dynamic_cast<Player*>(&actor);
        if (player != nullptr && !bumped) {
            hitBrickFromBelow(*player, *object, world);
            bumped = true;
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

    const bool wasIntact = !brick->isOpened();
    brick->hitBy(player);
    if (wasIntact && brick->canBeBroken() && brick->isOpened()) {
        emitAtCenter(world, VisualEventType::BrickBroken, *brick,
                     player.getDirection());
    }

    if (auto* specialBrick = dynamic_cast<SpecialBrick*>(brick)) {
        std::unique_ptr<Item> item = specialBrick->releaseItem();
        if (item != nullptr) {
            world.addItem(std::move(item));
        }
        return;
    }

}

void CollisionSystem::resolveInteractions(
    World& world,
    double dtSeconds
) const {
    Player& player = world.getPlayer();

    if (player.isAlive()) {
        resolvePlayerInteractions(world, player, dtSeconds);
    }

    resolveFireballHits(world);
    resolveEnemyHits(world);
}

void CollisionSystem::resolvePlayerInteractions(
    World& world,
    Player& player,
    double dtSeconds
) const {
    for (const auto& object : world.getObjects()) {
        auto* flag = dynamic_cast<Flag*>(object.get());
        if (flag == nullptr || flag->isCaptured() || !check(player, *flag)) {
            continue;
        }

        player.captureFlag(*flag);
        world.markLevelComplete();
    }

    for (const auto& item : world.getItems()) {
        if (item->isCollected() || !check(player, *item)) {
            continue;
        }

        player.collect(*item);
        if (const auto* coin = dynamic_cast<const Coin*>(item.get());
            coin != nullptr && item->isCollected()) {
            world.collectCoin(coin->getValue());
        }
    }

    // Xử lý cộng điểm khi stomp Enemy và xử lý va chạm với Enemy.
    for (const auto& actor : world.getActors()) {
        auto* enemy = dynamic_cast<Enemy*>(actor.get());
        if (enemy == nullptr || !enemy->isAlive() ||
            !check(player, *enemy)) {
            continue;
        }

        auto* koopa = dynamic_cast<Koopa*>(enemy);
        const bool wasShellIdle = koopa != nullptr && koopa->isShell();
        const double enemyTop = enemy->getBounds().y;
        const bool stomped =
            isStomp(player, *enemy, dtSeconds) && enemy->isStompable();

        if (stomped) {
            enemy->onStomped(player);
            player.placeOnGround(enemyTop - player.getBounds().height);
            player.bounceAfterStomp();
            if (!wasShellIdle) {
                world.addScore(kEnemyDefeatScore);
            }
        } else {
            enemy->onPlayerContact(player);
        }

        if (wasShellIdle && koopa->isSlidingShell()) {
            emitAtBottomCenter(world, VisualEventType::ShellKicked, *enemy,
                               enemy->getDirection());
        } else if (stomped && !wasShellIdle) {
            emitAtBottomCenter(world, VisualEventType::EnemyStomped, *enemy,
                               enemy->getDirection());
        }
    }
}

// Xử lý bắn fireball vào enemy
void CollisionSystem::resolveFireballHits(World& world) const {
    const auto& actors = world.getActors();

    for (const auto& projectile : actors) {
        auto* fireball = dynamic_cast<Fireball*>(projectile.get());
        if (fireball == nullptr || !fireball->isAlive()) {
            continue;
        }

        for (const auto& target : actors) {
            auto* enemy = dynamic_cast<Enemy*>(target.get());
            if (enemy == nullptr || !enemy->isAlive() ||
                !check(*fireball, *enemy)) {
                continue;
            }

            emitAtCenter(world, VisualEventType::FireballImpact, *fireball,
                         fireball->getDirection());
            fireball->destroy();

            if (!enemy->takesFireballDamage()) {
                break;
            }

            enemy->die();
            world.addScore(kEnemyDefeatScore);
            break;
        }
    }
}

void CollisionSystem::resolveEnemyHits(World& world) const {
    const auto& actors = world.getActors();

    for (const auto& attacker : actors) {
        auto* shell = dynamic_cast<Enemy*>(attacker.get());
        if (shell == nullptr || !shell->isDeadlyToEnemies()) {
            continue;
        }

        for (const auto& target : actors) {
            if (target.get() == attacker.get()) {
                continue;
            }

            auto* victim = dynamic_cast<Enemy*>(target.get());
            if (victim == nullptr || !victim->isAlive() ||
                victim->isDeadlyToEnemies() || !check(*shell, *victim)) {
                continue;
            }

            if (auto* boss = dynamic_cast<GorillaBoss*>(victim)) {
                auto* shellKoopa = dynamic_cast<Koopa*>(attacker.get());
                if (shellKoopa == nullptr) {
                    continue;
                }

                if (boss->onShellHit(*shellKoopa) == BossHitResult::Damaged) {
                    emitAtCenter(world, VisualEventType::ShellImpact, *boss,
                                 shell->getDirection());
                    shellKoopa->die();
                    world.addScore(kEnemyDefeatScore);
                }
                continue;
            }

            emitAtCenter(world, VisualEventType::ShellImpact, *victim,
                         shell->getDirection());
            victim->die();
            world.addScore(kEnemyDefeatScore);
        }
    }
}
