#include "controller/CollisionSystem.h"
#include "model/Brick.h"
#include "model/Enemy.h"
#include "model/Fireball.h"
#include "model/LevelData.h"
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
constexpr double kSpawnY = 550.0 - Player::kSmallHeight;
constexpr int kPlayerWidth = Player::kBodyWidth;
constexpr int kPlayerHeight = Player::kSmallHeight;

constexpr double kGroundTopY = 576.0;
constexpr double kStandY = kGroundTopY - kPlayerHeight;

void addPlatform(
    World& world,
    int firstColumn,
    int lastColumn,
    double topY
) {
    for (int column = firstColumn; column <= lastColumn; ++column) {
        world.addObject(std::make_unique<StandardBrick>(
            static_cast<double>(column * kTileSize), topY));
    }
}

void addGround(World& world, int firstColumn, int lastColumn) {
    addPlatform(world, firstColumn, lastColumn, kGroundTopY);
}

bool intersects(const Rectangle& a, const Rectangle& b) {
    return a.x < b.x + b.width &&
           a.x + a.width > b.x &&
           a.y < b.y + b.height &&
           a.y + a.height > b.y;
}

// Bất biến mạnh nhất sau mỗi frame: hộp vật lý không được nằm trong khối rắn.
void assertPlayerClearOfSolids(const World& world, const Player& player) {
    for (const auto& object : world.getObjects()) {
        if (object->isSolid()) {
            assert(!intersects(player.getBounds(), object->getBounds()));
        }
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
    bool standardBumped = false;
    double bonkVelocityY = -1.0;
    bool bonkOnGround = true;
    for (int index = 0; index < 20; ++index) {
        world.update(kStepSeconds);
        collisionSystem.update(world, kStepSeconds);
        standardBumped = standardBumped ||
                         standardBrick->getRenderOffsetY() < 0;

        // Đỉnh collider chạm đáy gạch, không phải đỉnh ô sprite.
        assert(player.getBounds().y >= 480.0);
        if (!bonked && player.getBounds().y == 480.0) {
            bonked = true;
            bonkVelocityY = player.getVelocityY();
            bonkOnGround = player.isOnGround();
        }
    }

    assert(bonked);
    assert(standardBumped);
    assert(bonkVelocityY == 0.0);
    assert(!bonkOnGround);

    assert(!standardBrick->isOpened());
    assert(standardBrick->getY() == 448.0);
    world.update(0.2);
    assert(standardBrick->getRenderOffsetY() == 0);

    World itemWorld;
    CollisionSystem itemCollisions;
    addGround(itemWorld, 0, 8);

    auto coinOwner = std::make_unique<CoinBrick>(96.0, 448.0, 1);
    const CoinBrick* coinBrick = coinOwner.get();
    itemWorld.addObject(std::move(coinOwner));

    landOnGround(itemWorld, itemCollisions);
    assert(itemWorld.getItems().empty());

    itemWorld.getPlayer().jump();
    bool coinBrickBumped = false;
    for (int index = 0; index < 40; ++index) {
        step(itemWorld, itemCollisions, 1);
        coinBrickBumped = coinBrickBumped ||
                          coinBrick->getRenderOffsetY() < 0;
    }
    assert(itemWorld.getItems().size() == 1);
    assert(coinBrick->isOpened());
    assert(coinBrickBumped);
    assert(coinBrick->getY() == 448.0);

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

    // E: chặn theo cạnh collider; sprite được phép phủ lên viền trang trí.
    constexpr double kRightWallLeft = 192.0;
    constexpr double kLeftWallRight = 64.0;

    Player& player = world.getPlayer();
    for (int index = 0; index < 100; ++index) {
        stepHolding(world, collisionSystem, 1, 1);
        const Rectangle body = player.getBounds();
        assert(body.x + body.width <= kRightWallLeft);
        assert(player.getY() == kStandY);
        assert(player.isOnGround());
    }
    assert(player.getBounds().x + player.getBounds().width == kRightWallLeft);
    assert(player.getX() ==
           kRightWallLeft - Player::kColliderInsetX - Player::kColliderWidth);

    for (int index = 0; index < 100; ++index) {
        stepHolding(world, collisionSystem, 1, -1);
        assert(player.getBounds().x >= kLeftWallRight);
        assert(player.getY() == kStandY);
        assert(player.isOnGround());
    }
    assert(player.getBounds().x == kLeftWallRight);
    assert(player.getX() == kLeftWallRight - Player::kColliderInsetX);
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
    const double restingX =
        192.0 - Player::kColliderInsetX - Player::kColliderWidth;
    assert(player.getX() == restingX);

    player.jump();
    double apexTopY = player.getY();
    for (int index = 0; index < 120; ++index) {
        world.getPlayer().setMoveDirection(1);
        world.update(kStepSeconds);
        collisionSystem.update(world, kStepSeconds);

        assert(player.getX() == restingX);
        if (player.getVelocityY() < 0.0) {
            assert(!player.isOnGround());
        }
        assert(player.getY() + kPlayerHeight > 384.0);
        apexTopY = (player.getY() < apexTopY) ? player.getY() : apexTopY;
    }

    assert(apexTopY < kStandY - 90.0);
    assert(player.getY() == kStandY);
    assert(player.isOnGround());
    assert(player.getX() == restingX);
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

void addLowCeiling(World& world, int firstColumn, int lastColumn) {
    for (int column = firstColumn; column <= lastColumn; ++column) {
        world.addObject(std::make_unique<StandardBrick>(
            static_cast<double>(column * kTileSize), 512.0));
    }
}

void testPlayerSpawnMarker() {
    LevelData level(10, 8, kTileSize);
    for (int column = 0; column < 10; ++column) {
        level.setTile(column, 7, kStandardBrickTileId);
    }
    level.setTile(6, 6, kPlayerSpawnTileId);

    World world;
    CollisionSystem collisionSystem;
    world.loadLevel(level);

    Player& player = world.getPlayer();
    assert(player.getX() == 6.0 * kTileSize);
    assert(player.getY() + player.getHeight() == 7.0 * kTileSize);
    assert(player.getState() == PlayerState::Small);

    step(world, collisionSystem, 20);
    assert(player.isOnGround());
    assert(player.getY() + player.getHeight() == 7.0 * kTileSize);
    assert(!world.isGameOver());

    // Marker thứ hai dời spawn thay vì tạo thêm chỗ spawn.
    level.setTile(2, 6, kPlayerSpawnTileId);
    assert(level.getTile(6, 6) == kEmptyTileId);
    world.loadLevel(level);
    assert(world.getPlayer().getX() == 2.0 * kTileSize);
    assert(world.getPlayer().getY() + world.getPlayer().getHeight() ==
           7.0 * kTileSize);

    World withoutMarker;
    LevelData plain(10, 8, kTileSize);
    for (int column = 0; column < 10; ++column) {
        plain.setTile(column, 7, kStandardBrickTileId);
    }
    const double defaultX = withoutMarker.getPlayer().getX();
    const double defaultY = withoutMarker.getPlayer().getY();
    withoutMarker.loadLevel(plain);
    assert(withoutMarker.getPlayer().getX() == defaultX);
    assert(withoutMarker.getPlayer().getY() == defaultY);
}

void testLifeLossPreservesLevelState() {
    LevelData level(6, 20, kTileSize);
    level.setTile(1, 1, kCoinTileId);
    level.setTile(2, 1, kCoinBrickTileId);
    level.setTile(4, 1, kGoombaTileId);

    World world;
    world.loadLevel(level);
    assert(world.getRemainingCoins() == 2);
    assert(world.getTimeRemaining() == 600);
    assert(world.getLives() == 3);

    world.collectCoin(Coin::kScoreValue);
    assert(world.getScore() == 200);
    assert(world.getRemainingCoins() == 1);

    const Actor* enemy = world.getActors().front().get();
    const double enemyX = enemy->getX();
    const double playerY = world.getPlayer().getY();
    constexpr double cameraLeftX = 256.0;
    world.getPlayer().takeDamage();
    world.update(0.0, cameraLeftX);
    assert(!world.isGameOver());
    assert(world.getLives() == 2);
    assert(world.getPlayer().isAlive());
    assert(world.getPlayer().getX() == cameraLeftX);
    assert(world.getPlayer().getY() == playerY);
    assert(world.getScore() == 200);
    assert(world.getRemainingCoins() == 1);
    assert(world.getTimeRemaining() == 600);
    assert(world.getActors().size() == 1);
    assert(world.getActors().front().get() == enemy);
    assert(enemy->getX() == enemyX);

    world.update(0.0);
    assert(world.getLives() == 2);

    World fallWorld;
    const double safeY = fallWorld.getPlayer().getY();
    fallWorld.getPlayer().reviveAt(400.0, 1000.0);
    fallWorld.update(0.0, cameraLeftX);
    assert(fallWorld.getLives() == 2);
    assert(fallWorld.getPlayer().getX() == cameraLeftX);
    assert(fallWorld.getPlayer().getY() == safeY);
}

void testFireballRequiresFirePowerAndBouncesOnce() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 30);

    assert(!world.shootFireball());
    world.getPlayer().upgradeToFire();
    assert(world.shootFireball());
    assert(world.getActors().size() == 1);

    int upwardTransitions = 0;
    double previousVelocityY = 0.0;
    for (int index = 0;
         index < 400 && !world.getActors().empty();
         ++index) {
        world.update(kStepSeconds);
        collisionSystem.update(world, kStepSeconds);

        if (!world.getActors().empty()) {
            const auto* fireball =
                dynamic_cast<const Fireball*>(world.getActors().front().get());
            assert(fireball != nullptr);
            if (previousVelocityY >= 0.0 && fireball->getVelocityY() < 0.0) {
                ++upwardTransitions;
            }
            previousVelocityY = fireball->getVelocityY();
        }
    }

    assert(upwardTransitions == 1);
    assert(world.getActors().empty());
}

void testFireballDefeatsEnemy() {
    World world;
    CollisionSystem collisionSystem;
    world.addActor(std::make_unique<Fireball>(
        300.0, 544.0, Direction::Right));
    world.addActor(std::make_unique<Goomba>(318.0, 544.0));

    world.update(kStepSeconds);
    collisionSystem.update(world, kStepSeconds);
    assert(world.getScore() == 100);

    world.update(0.0);
    assert(world.getActors().empty());
}

void testCoinScoreAndTimeBonusApplyOnce() {
    World coinWorld;
    CollisionSystem collisionSystem;
    coinWorld.addItem(std::make_unique<Coin>(
        108.0, 524.0, Coin::kScoreValue));

    collisionSystem.update(coinWorld, 0.0);
    assert(coinWorld.getScore() == 200);
    collisionSystem.update(coinWorld, 0.0);
    assert(coinWorld.getScore() == 200);

    World timedWorld;
    timedWorld.update(10.0);
    assert(timedWorld.getTimeRemaining() == 590);
    timedWorld.markLevelComplete();
    assert(timedWorld.getScore() == 5900);
    timedWorld.markLevelComplete();
    assert(timedWorld.getScore() == 5900);

    World expiredWorld;
    expiredWorld.update(600.0);
    assert(expiredWorld.getTimeRemaining() == 600);
    assert(!expiredWorld.isGameOver());
    assert(expiredWorld.getPlayer().isAlive());
    assert(expiredWorld.getLives() == 2);
    expiredWorld.update(1.0);
    assert(expiredWorld.getLives() == 2);
    assert(expiredWorld.getTimeRemaining() == 599);

    World livesWorld;
    for (int expectedLives = 2; expectedLives >= 0; --expectedLives) {
        livesWorld.getPlayer().takeDamage();
        livesWorld.update(0.0);
        assert(livesWorld.getLives() == expectedLives);
        assert(livesWorld.isGameOver() == (expectedLives == 0));
        if (expectedLives > 0) {
            assert(livesWorld.getPlayer().isAlive());
            livesWorld.update(2.0);
        }
    }
}

void testPlayerHitboxTracksState() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 10);
    addLowCeiling(world, 5, 10);

    Player& player = world.getPlayer();
    landOnGround(world, collisionSystem);
    assert(player.getWidth() == Player::kBodyWidth);
    assert(player.getHeight() == Player::kSmallHeight);

    // Hộp vật lý hẹp hơn ô sprite nhưng chung đáy.
    const Rectangle smallBody = player.getBounds();
    assert(smallBody.width == Player::kColliderWidth);
    assert(smallBody.width < player.getWidth());
    assert(smallBody.x == player.getX() + Player::kColliderInsetX);
    assert(smallBody.y + smallBody.height ==
           player.getY() + player.getHeight());

    const Rectangle feet = player.getFeetBounds();
    assert(feet.y + feet.height == smallBody.y + smallBody.height);
    assert(feet.width < smallBody.width);

    stepHolding(world, collisionSystem, 60, 1);
    assert(player.getX() > 160.0);
    assert(player.getY() == kStandY);
    assert(player.isOnGround());

    World bigWorld;
    CollisionSystem bigCollisions;
    addGround(bigWorld, 0, 10);
    addLowCeiling(bigWorld, 5, 10);

    Player& big = bigWorld.getPlayer();
    landOnGround(bigWorld, bigCollisions);
    const double feetY = big.getY() + big.getHeight();
    assert(big.getBounds().y + big.getBounds().height == feetY);

    big.grow();
    assert(big.getState() == PlayerState::Big);
    assert(big.getHeight() == Player::kBigHeight);
    assert(big.getY() + big.getHeight() == feetY);

    // I: đáy collider đứng yên khi đổi state, chỉ đỉnh cao thêm.
    const Rectangle bigBody = big.getBounds();
    assert(bigBody.y + bigBody.height == feetY);
    assert(bigBody.width == smallBody.width);
    assert(bigBody.height > smallBody.height);

    stepHolding(bigWorld, bigCollisions, 60, 1);
    assert(big.getBounds().x + big.getBounds().width <= 160.0);
    assert(big.getY() + big.getHeight() == feetY);

    big.takeDamage();
    assert(big.getState() == PlayerState::Small);
    assert(big.getHeight() == Player::kSmallHeight);
    assert(big.getY() + big.getHeight() == feetY);
    assert(big.getBounds().height == smallBody.height);
    assert(big.getBounds().y + big.getBounds().height == feetY);

    // FIRE dùng chung collider với BIG.
    World fireWorld;
    CollisionSystem fireCollisions;
    addGround(fireWorld, 0, 10);

    Player& fire = fireWorld.getPlayer();
    landOnGround(fireWorld, fireCollisions);
    const double fireFeetY = fire.getY() + fire.getHeight();

    fire.upgradeToFire();
    assert(fire.getState() == PlayerState::Fire);
    assert(fire.getHeight() == Player::kBigHeight);
    assert(fire.getBounds().height == bigBody.height);
    assert(fire.getBounds().width == bigBody.width);
    assert(fire.getBounds().y + fire.getBounds().height == fireFeetY);
}

// B/C: đứng được chừng nào chân còn chồng nền, không đòi tâm nằm trên block.
void testStandsOnPlatformEdgeThenFalls() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 4);
    landOnGround(world, collisionSystem);

    constexpr double kPlatformRightX = 5.0 * kTileSize;

    Player& player = world.getPlayer();
    bool overhungPastCentre = false;
    bool fell = false;

    for (int index = 0; index < 200 && !fell; ++index) {
        stepHolding(world, collisionSystem, 1, 1);
        assertPlayerClearOfSolids(world, player);

        const Rectangle body = player.getBounds();
        if (player.isOnGround()) {
            assert(body.x < kPlatformRightX);
            assert(player.getY() == kStandY);
            overhungPastCentre = overhungPastCentre ||
                                 body.x + body.width / 2.0 > kPlatformRightX;
        } else {
            assert(body.x >= kPlatformRightX);
            fell = true;
        }
    }

    assert(fell);
    assert(overhungPastCentre);
}

// D: vừa đi ngang vừa rơi xuống mép nền thấp hơn.
void testFallsOntoPlatformCornerWhileMoving() {
    World world;
    CollisionSystem collisionSystem;
    addPlatform(world, 0, 3, 550.0);
    addPlatform(world, 5, 14, kGroundTopY);

    Player& player = world.getPlayer();
    step(world, collisionSystem, 10);
    assert(player.isOnGround());
    assert(player.getY() == 550.0 - kPlayerHeight);

    double previousX = player.getX();
    double previousY = player.getY();
    bool leftLedge = false;

    for (int index = 0; index < 60; ++index) {
        stepHolding(world, collisionSystem, 1, 1);
        assertPlayerClearOfSolids(world, player);

        const double deltaX = player.getX() - previousX;
        assert(deltaX >= 0.0);
        assert(deltaX <= 3.0);  // không teleport ngang khi chạm góc

        if (player.getVelocityY() > 0.0) {
            assert(player.getY() >= previousY);  // không bị đẩy ngược lên
        }

        leftLedge = leftLedge || !player.isOnGround();
        previousX = player.getX();
        previousY = player.getY();
    }

    assert(leftLedge);
    assert(player.isOnGround());   // không lơ lửng
    assert(player.getY() == kStandY);
    assert(player.getX() > 128.0);  // không kẹt lại ở mép trên
}

// G: nhảy sát góc gạch - chỉ đội khi collider thật sự chui vào, và không bị
// hất ngang.
void testJumpAtBrickCorner() {
    World clearWorld;
    CollisionSystem clearCollisions;
    addGround(clearWorld, 0, 8);
    auto missOwner = std::make_unique<CoinBrick>(128.0, 448.0, 1);
    const CoinBrick* missedBrick = missOwner.get();
    clearWorld.addObject(std::move(missOwner));

    landOnGround(clearWorld, clearCollisions);
    Player& missPlayer = clearWorld.getPlayer();
    const double missStartX = missPlayer.getX();
    assert(missPlayer.getBounds().x + missPlayer.getBounds().width <= 128.0);

    missPlayer.jump();
    for (int index = 0; index < 120; ++index) {
        step(clearWorld, clearCollisions, 1);
        assertPlayerClearOfSolids(clearWorld, missPlayer);
        assert(missPlayer.getX() == missStartX);
    }

    assert(!missedBrick->isOpened());
    assert(clearWorld.getItems().empty());

    World grazeWorld;
    CollisionSystem grazeCollisions;
    addGround(grazeWorld, 0, 8);
    auto grazeOwner = std::make_unique<CoinBrick>(120.0, 448.0, 1);
    const CoinBrick* grazedBrick = grazeOwner.get();
    grazeWorld.addObject(std::move(grazeOwner));

    landOnGround(grazeWorld, grazeCollisions);
    Player& grazePlayer = grazeWorld.getPlayer();
    const double grazeStartX = grazePlayer.getX();
    assert(grazePlayer.getBounds().x + grazePlayer.getBounds().width > 120.0);

    grazePlayer.jump();
    for (int index = 0; index < 120; ++index) {
        step(grazeWorld, grazeCollisions, 1);
        assertPlayerClearOfSolids(grazeWorld, grazePlayer);
        assert(grazePlayer.getX() == grazeStartX);
    }

    assert(grazedBrick->isOpened());
    assert(grazeWorld.getItems().size() == 1);
}

// H: đổi Idle/Run/Jump/Fall không được đổi hình dạng va chạm.
void testColliderIgnoresAnimationPhase() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 12);
    landOnGround(world, collisionSystem);

    Player& player = world.getPlayer();
    const Rectangle idle = player.getBounds();

    stepHolding(world, collisionSystem, 20, 1);
    const Rectangle running = player.getBounds();
    assert(player.getVelocityX() > 0.0);

    player.jump();
    step(world, collisionSystem, 5);
    const Rectangle rising = player.getBounds();
    assert(player.getVelocityY() < 0.0);

    bool falling = false;
    for (int index = 0; index < 200 && !falling; ++index) {
        step(world, collisionSystem, 1);
        falling = player.getVelocityY() > 0.0 && !player.isOnGround();
    }
    assert(falling);
    const Rectangle dropping = player.getBounds();

    for (const Rectangle& shape : {running, rising, dropping}) {
        assert(shape.width == idle.width);
        assert(shape.height == idle.height);
    }

    // Và luôn dính đáy hộp logic ở mọi pha.
    assert(dropping.y + dropping.height ==
           player.getY() + player.getHeight());
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
void addLowGround(World& world, int firstColumn, int lastColumn) {
    for (int column = firstColumn; column <= lastColumn; ++column) {
        world.addObject(std::make_unique<StandardBrick>(
            static_cast<double>(column * kTileSize), 640.0));
    }
}

void testKoopaStompBecomesShell() {
    World world;
    CollisionSystem collisionSystem;
    addLowGround(world, 0, 12);

    auto koopaOwner = std::make_unique<Koopa>(
        kSpawnX, 640.0 - Koopa::kWalkHeight, KoopaColor::Green);
    const Koopa* koopa = koopaOwner.get();
    world.addActor(std::move(koopaOwner));

    bool becameShell = false;
    for (int index = 0; index < 60 && !becameShell; ++index) {
        step(world, collisionSystem, 1);
        becameShell = koopa->isShell();
    }

    assert(becameShell);
    assert(koopa->isAlive());
    assert(koopa->getHeight() == Koopa::kShellHeight);
    assert(koopa->getY() + koopa->getHeight() == 640.0);
    assert(koopa->getVelocityX() == 0.0);
    assert(!koopa->isDeadlyToEnemies());
    assert(world.getScore() == 100);
    assert(world.getPlayer().getState() == PlayerState::Small);
}

void testKoopaShellSlidesAndKillsEnemy() {
    World world;
    CollisionSystem collisionSystem;
    addLowGround(world, 0, 14);
    world.addObject(std::make_unique<StandardBrick>(448.0, 608.0));

    auto koopaOwner = std::make_unique<Koopa>(
        192.0, 640.0 - Koopa::kWalkHeight, KoopaColor::Green);
    Koopa* koopa = koopaOwner.get();
    world.addActor(std::move(koopaOwner));
    world.addActor(std::make_unique<Goomba>(320.0, 608.0));

    step(world, collisionSystem, 5);
    koopa->hideInShell();
    koopa->kick(Direction::Right);
    assert(koopa->isSlidingShell());
    assert(koopa->isDeadlyToEnemies());

    step(world, collisionSystem, 1);
    assert(koopa->getVelocityX() > 0.0);

    bool goombaGone = false;
    for (int index = 0; index < 120 && !goombaGone; ++index) {
        step(world, collisionSystem, 1);
        goombaGone = world.getActors().size() == 1;
    }

    assert(goombaGone);
    assert(world.getActors().front().get() == koopa);
    assert(koopa->isSlidingShell());
    assert(world.getScore() >= 100);

    step(world, collisionSystem, 60);
    assert(koopa->getX() + Koopa::kWalkWidth <= 448.0);
    assert(koopa->getDirection() == Direction::Left);
}

void testPiranhaCycleNotStompable() {
    World world;
    CollisionSystem collisionSystem;
    addGround(world, 0, 8);

    auto plantOwner = std::make_unique<PiranhaPlant>(kSpawnX, kGroundTopY);
    const PiranhaPlant* plant = plantOwner.get();
    world.addActor(std::move(plantOwner));

    Player& player = world.getPlayer();
    assert(!plant->isStompable());
    assert(!plant->shouldTurnAtEdge());
    assert(plant->getPhase() == PiranhaPhase::Hidden);
    assert(plant->getHeight() == 0);

    for (int index = 0; index < 100; ++index) {
        step(world, collisionSystem, 1);
        assert(plant->getX() == kSpawnX);
        assert(plant->getY() + plant->getHeight() == kGroundTopY);
        assert(plant->getVelocityY() == 0.0);
    }

    assert(plant->getPhase() == PiranhaPhase::Hidden);
    assert(player.isAlive());
    assert(player.isOnGround());
    assert(player.getY() == kStandY);

    bool fullyExposed = false;
    for (int index = 0; index < 400 && !fullyExposed; ++index) {
        step(world, collisionSystem, 1);
        assert(plant->getY() + plant->getHeight() == kGroundTopY);
        fullyExposed = plant->getHeight() >= PiranhaPlant::kPlantHeight;
    }

    assert(fullyExposed);
    assert(plant->getPhase() == PiranhaPhase::Exposed);
    assert(plant->isAlive());
    assert(player.isAlive());
    assert(world.getLives() == 2);
    assert(world.getScore() == 0);
    player.reviveAt(300.0, kStandY);

    bool hiddenAgain = false;
    for (int index = 0; index < 400 && !hiddenAgain; ++index) {
        step(world, collisionSystem, 1);
        hiddenAgain = plant->getPhase() == PiranhaPhase::Hidden;
    }

    assert(hiddenAgain);
    assert(plant->getHeight() == 0);
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

void testRedKoopaTurnsAtEdgeGreenDoesNot() {
    World redWorld;
    CollisionSystem redCollisions;
    addGround(redWorld, 2, 4);
    addGround(redWorld, 6, 10);
    redWorld.addActor(std::make_unique<Koopa>(
        256.0, kGroundTopY - Koopa::kWalkHeight, KoopaColor::Red));

    const PatrolStats stats =
        measurePatrol(redWorld, redCollisions, 500, 192.0, 352.0);
    assert(stats.turns >= 2);

    World greenWorld;
    CollisionSystem greenCollisions;
    addGround(greenWorld, 2, 4);
    addGround(greenWorld, 6, 10);
    auto greenOwner = std::make_unique<Koopa>(
        256.0, kGroundTopY - Koopa::kWalkHeight, KoopaColor::Green);
    const Koopa* green = greenOwner.get();
    greenWorld.addActor(std::move(greenOwner));

    bool leftPlatform = false;
    for (int index = 0; index < 500 && !leftPlatform; ++index) {
        step(greenWorld, greenCollisions, 1);
        leftPlatform = !green->isOnGround();
    }

    assert(leftPlatform);
    assert(green->getX() < 192.0);
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
    testPlayerSpawnMarker();
    testLifeLossPreservesLevelState();
    testFireballRequiresFirePowerAndBouncesOnce();
    testFireballDefeatsEnemy();
    testCoinScoreAndTimeBonusApplyOnce();
    testPlayerHitboxTracksState();
    testStandsOnPlatformEdgeThenFalls();
    testFallsOntoPlatformCornerWhileMoving();
    testJumpAtBrickCorner();
    testColliderIgnoresAnimationPhase();
    testStompSurvivesAxisResolve();
    testEnemyTurnsAtWall();
    testEnemyTurnsAtEdge();
    testEnemyWallAndEdgeSameFrame();
    testKoopaStompBecomesShell();
    testKoopaShellSlidesAndKillsEnemy();
    testRedKoopaTurnsAtEdgeGreenDoesNot();
    testPiranhaCycleNotStompable();

    std::cout << "Axis collision resolve passed\n";
    return 0;
}
