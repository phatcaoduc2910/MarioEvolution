#include "controller/CollisionSystem.h"
#include "model/Brick.h"
#include "model/Enemy.h"
#include "model/Player.h"
#include "model/World.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <utility>

namespace {
constexpr double kStepSeconds = 0.011;
constexpr int kTileSize = 32;

constexpr double kSpawnX = 100.0;
constexpr double kSpawnY = 502.0;
constexpr int kPlayerWidth = 32;
constexpr int kPlayerHeight = 48;

constexpr double kGroundTopY = 576.0;
constexpr double kStandY = kGroundTopY - kPlayerHeight;

void addGround(World& world, int firstColumn, int lastColumn) {
    for (int column = firstColumn; column <= lastColumn; ++column) {
        world.addObject(std::make_unique<StandardBrick>(
            static_cast<double>(column * kTileSize), kGroundTopY));
    }
}

void step(World& world, CollisionSystem& collisionSystem, int steps) {
    for (int index = 0; index < steps; ++index) {
        world.update(kStepSeconds);
        collisionSystem.update(world, kStepSeconds);
    }
}

void stepHolding(
    World& world,
    CollisionSystem& collisionSystem,
    int steps,
    int moveDirection
) {
    for (int index = 0; index < steps; ++index) {
        world.getPlayer().setMoveDirection(moveDirection);
        world.update(kStepSeconds);
        collisionSystem.update(world, kStepSeconds);
    }
}

void landOnGround(World& world, CollisionSystem& collisionSystem) {
    step(world, collisionSystem, 30);
    assert(world.getPlayer().isOnGround());
    assert(world.getPlayer().getY() == kStandY);
}

void testFallToGround() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 8);

    Player& player = world.getPlayer();
    assert(!player.isOnGround());
    assert(player.getY() == kSpawnY);

    for (int index = 0; index < 30; ++index) {
        world.update(kStepSeconds);
        collisionSystem.update(world, kStepSeconds);

        assert(player.getY() + kPlayerHeight <= kGroundTopY);
    }

    assert(player.getY() > kSpawnY);
    assert(player.getY() == kStandY);
    assert(player.getY() + kPlayerHeight == kGroundTopY);
    assert(player.getVelocityY() == 0.0);
    assert(player.isOnGround());
    assert(player.getX() == kSpawnX);
}

void testStandStill() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 8);
    landOnGround(world, collisionSystem);

    Player& player = world.getPlayer();
    for (int index = 0; index < 200; ++index) {
        world.update(kStepSeconds);
        collisionSystem.update(world, kStepSeconds);

        assert(player.getY() == kStandY);
        assert(player.getX() == kSpawnX);
        assert(player.getVelocityY() == 0.0);
        assert(player.isOnGround());
    }
}

void testHeadIntoBrick() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 8);

    auto standardOwner = std::make_unique<StandardBrick>(96.0, 448.0);
    const StandardBrick* standardBrick = standardOwner.get();
    world.addObject(std::move(standardOwner));

    landOnGround(world, collisionSystem);

    Player& player = world.getPlayer();
    player.jump();
    assert(player.getVelocityY() < 0.0);

    bool bonked = false;
    double bonkVelocityY = -1.0;
    bool bonkOnGround = true;
    for (int index = 0; index < 20; ++index) {
        world.update(kStepSeconds);
        collisionSystem.update(world, kStepSeconds);

        assert(player.getY() >= 480.0);
        if (!bonked && player.getY() == 480.0) {
            bonked = true;
            bonkVelocityY = player.getVelocityY();
            bonkOnGround = player.isOnGround();
        }
    }

    assert(bonked);
    assert(bonkVelocityY == 0.0);
    assert(!bonkOnGround);

    assert(!standardBrick->isOpened());

    World itemWorld;
    CollisionSystem itemCollisions;
    addGround(itemWorld, 0, 8);

    auto coinOwner = std::make_unique<CoinBrick>(96.0, 448.0, 1);
    const CoinBrick* coinBrick = coinOwner.get();
    itemWorld.addObject(std::move(coinOwner));

    landOnGround(itemWorld, itemCollisions);
    assert(itemWorld.getItems().empty());

    itemWorld.getPlayer().jump();
    step(itemWorld, itemCollisions, 40);
    assert(itemWorld.getItems().size() == 1);
    assert(coinBrick->isOpened());

    assert(itemWorld.getPlayer().isOnGround());
    itemWorld.getPlayer().jump();
    step(itemWorld, itemCollisions, 40);
    assert(itemWorld.getItems().size() == 1);
}

void testWalkIntoWalls() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 8);

    world.addObject(std::make_unique<StandardBrick>(192.0, 544.0));
    world.addObject(std::make_unique<StandardBrick>(32.0, 544.0));

    landOnGround(world, collisionSystem);

    Player& player = world.getPlayer();
    for (int index = 0; index < 100; ++index) {
        stepHolding(world, collisionSystem, 1, 1);
        assert(player.getX() + kPlayerWidth <= 192.0);
        assert(player.getY() == kStandY);
        assert(player.isOnGround());
    }
    assert(player.getX() == 160.0);

    for (int index = 0; index < 100; ++index) {
        stepHolding(world, collisionSystem, 1, -1);
        assert(player.getX() >= 64.0);
        assert(player.getY() == kStandY);
        assert(player.isOnGround());
    }
    assert(player.getX() == 64.0);
}

void testJumpBesideCorner() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 8);

    for (double wallY = 384.0; wallY < kGroundTopY; wallY += kTileSize) {
        world.addObject(std::make_unique<StandardBrick>(192.0, wallY));
    }

    landOnGround(world, collisionSystem);
    stepHolding(world, collisionSystem, 40, 1);

    Player& player = world.getPlayer();
    assert(player.getX() == 160.0);

    player.jump();
    double apexTopY = player.getY();
    for (int index = 0; index < 120; ++index) {
        world.getPlayer().setMoveDirection(1);
        world.update(kStepSeconds);
        collisionSystem.update(world, kStepSeconds);

        assert(player.getX() == 160.0);
        if (player.getVelocityY() < 0.0) {
            assert(!player.isOnGround());
        }
        assert(player.getY() + kPlayerHeight > 384.0);
        apexTopY = (player.getY() < apexTopY) ? player.getY() : apexTopY;
    }

    assert(apexTopY < 430.0);
    assert(player.getY() == kStandY);
    assert(player.isOnGround());
    assert(player.getX() == 160.0);
}

void testFallBesideBlock() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 8);

    world.addObject(std::make_unique<StandardBrick>(68.0, 512.0));
    world.addObject(std::make_unique<StandardBrick>(132.0, 512.0));

    Player& player = world.getPlayer();
    for (int index = 0; index < 40; ++index) {
        world.update(kStepSeconds);
        collisionSystem.update(world, kStepSeconds);

        assert(player.getX() == kSpawnX);
        assert(player.getVelocityX() == 0.0);
        assert(player.getY() + kPlayerHeight <= kGroundTopY);
    }

    assert(player.getY() == kStandY);
    assert(player.isOnGround());
}

void testStompSurvivesAxisResolve() {
    World world;
    CollisionSystem collisionSystem;

    for (int column = 0; column <= 8; ++column) {
        world.addObject(std::make_unique<StandardBrick>(
            static_cast<double>(column * kTileSize), 640.0));
    }
    world.addActor(std::make_unique<Goomba>(kSpawnX, 608.0));

    Player& player = world.getPlayer();
    step(world, collisionSystem, 40);

    assert(world.getActors().empty());
    assert(world.getScore() == 100);
    assert(player.isAlive());
    assert(player.getState() == PlayerState::Small);
}

void testEnemyTurnsAtWall() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 12);

    world.addObject(std::make_unique<StandardBrick>(192.0, 544.0));
    world.addObject(std::make_unique<StandardBrick>(320.0, 544.0));
    world.addActor(std::make_unique<Goomba>(256.0, 544.0));

    assert(world.getActors().front()->getDirection() == Direction::Left);

    Direction previousDirection = Direction::Left;
    int turns = 0;
    for (int index = 0; index < 400; ++index) {
        step(world, collisionSystem, 1);

        assert(world.getActors().size() == 1);
        const Actor& goomba = *world.getActors().front();
        assert(goomba.getX() >= 224.0);
        assert(goomba.getX() + kTileSize <= 320.0);
        assert(goomba.isOnGround());
        assert(goomba.getY() + kTileSize == kGroundTopY);

        if (goomba.getDirection() != previousDirection) {
            ++turns;
            previousDirection = goomba.getDirection();
        }
    }

    assert(turns >= 2);
}

struct PatrolStats {
    int turns;
    double minX;
    double maxX;
};

constexpr double kEdgeOverhangTolerance = 1.0;

PatrolStats measurePatrol(World& world, CollisionSystem& collisionSystem,
                          int steps, double leftLimit, double rightLimit) {
    const Actor& firstFrame = *world.getActors().front();
    Direction previousDirection = firstFrame.getDirection();
    PatrolStats stats{0, firstFrame.getX(), firstFrame.getX()};

    for (int index = 0; index < steps; ++index) {
        step(world, collisionSystem, 1);

        assert(world.getActors().size() == 1);
        const Actor& enemy = *world.getActors().front();
        assert(enemy.isOnGround());
        assert(enemy.getX() >= leftLimit - kEdgeOverhangTolerance);
        assert(enemy.getX() + kTileSize <= rightLimit + kEdgeOverhangTolerance);

        stats.minX = (enemy.getX() < stats.minX) ? enemy.getX() : stats.minX;
        stats.maxX = (enemy.getX() > stats.maxX) ? enemy.getX() : stats.maxX;

        if (enemy.getDirection() != previousDirection) {
            ++stats.turns;
            previousDirection = enemy.getDirection();
        }
    }

    return stats;
}

void testEnemyTurnsAtEdge() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 2, 4);
    addGround(world, 6, 10);
    world.addActor(std::make_unique<Goomba>(256.0, 544.0));

    const PatrolStats stats =
        measurePatrol(world, collisionSystem, 500, 192.0, 352.0);
    assert(stats.turns >= 2);
    assert(stats.maxX - stats.minX > 96.0);
}

void testEnemyWallAndEdgeSameFrame() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 2, 4);
    addGround(world, 6, 7);
    world.addObject(std::make_unique<StandardBrick>(256.0, 544.0));
    world.addActor(std::make_unique<Goomba>(192.0, 544.0));

    const PatrolStats stats =
        measurePatrol(world, collisionSystem, 400, 192.0, 256.0);
    assert(stats.turns >= 2);
    assert(stats.maxX - stats.minX > 16.0);
}
}

int main() {
    testFallToGround();
    testStandStill();
    testHeadIntoBrick();
    testWalkIntoWalls();
    testJumpBesideCorner();
    testFallBesideBlock();
    testStompSurvivesAxisResolve();
    testEnemyTurnsAtWall();
    testEnemyTurnsAtEdge();
    testEnemyWallAndEdgeSameFrame();

    std::cout << "Axis collision resolve passed\n";
    return 0;
}
