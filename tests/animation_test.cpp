#include "model/Enemy.h"
#include "model/Fireball.h"
#include "model/Player.h"
#include "model/World.h"
#include "view/EffectManager.h"
#include "view/PlayerAnimationState.h"
#include "view/SpriteAnimation.h"

#include <cassert>
#include <cstddef>
#include <iostream>

namespace {
constexpr double kStepSeconds = 0.011;
constexpr int kFrameMs = 16;

void testLoopAnimationKeepsCycling() {
    SpriteAnimation animation({"a", "b", "c"}, 100);

    assert(animation.getCurrentFrameId() == "a");
    animation.update(100);
    assert(animation.getCurrentFrameId() == "b");
    animation.update(200);
    assert(animation.getCurrentFrameId() == "a");
    assert(!animation.isFinished());
    assert(animation.getTotalDurationMs() == 300);
}

void testTransientAnimationFinishesOnLastFrame() {
    SpriteAnimation animation({"a", "b"}, 100, false);

    animation.update(100);
    assert(animation.getCurrentFrameId() == "b");
    assert(!animation.isFinished());

    animation.update(100);
    assert(animation.isFinished());

    animation.update(1000);
    assert(animation.getCurrentFrameId() == "b");
    assert(animation.isFinished());

    animation.reset();
    assert(!animation.isFinished());
    assert(animation.getCurrentFrameId() == "a");
}

void testSingleFrameTransientFinishes() {
    SpriteAnimation animation({"only"}, 120, false);

    assert(!animation.isFinished());
    animation.update(120);
    assert(animation.isFinished());
    assert(animation.getCurrentFrameId() == "only");
}

void testAirborneSplitsJumpAndFall() {
    Player player(0.0, 0.0);

    player.placeOnGround(player.getBounds().y);
    player.jump();
    assert(!player.isOnGround());
    player.update(kStepSeconds);
    assert(player.getVelocityY() < 0.0);
    assert(selectPlayerAnimationState(player, false, false) ==
           PlayerAnimationState::Jump);

    for (int index = 0; index < 100 && player.getVelocityY() <= 0.0; ++index) {
        player.update(kStepSeconds);
    }
    assert(player.getVelocityY() > 0.0);
    assert(!player.isOnGround());
    assert(selectPlayerAnimationState(player, false, false) ==
           PlayerAnimationState::Fall);
}

void testGroundStatesAndPriority() {
    Player player(0.0, 0.0);

    player.placeOnGround(player.getBounds().y);
    assert(player.isOnGround());
    assert(selectPlayerAnimationState(player, false, false) ==
           PlayerAnimationState::Idle);

    player.setMoveDirection(1);
    player.update(kStepSeconds);
    assert(selectPlayerAnimationState(player, false, false) ==
           PlayerAnimationState::Run);

    for (int index = 0; index < 40; ++index) {
        player.setMoveDirection(1);
        player.update(kStepSeconds);
        player.placeOnGround(player.getBounds().y);
    }
    player.setMoveDirection(-1);
    assert(isPlayerSkidding(player));
    assert(selectPlayerAnimationState(player, false, false) ==
           PlayerAnimationState::Skid);

    assert(selectPlayerAnimationState(player, false, true) ==
           PlayerAnimationState::Hurt);
    assert(selectPlayerAnimationState(player, true, true) ==
           PlayerAnimationState::Transform);

    player.takeDamage();
    assert(!player.isAlive());
    assert(selectPlayerAnimationState(player, true, true) ==
           PlayerAnimationState::Death);
}

void testTurnLatchOutlivesSkidCondition() {
    Player player(0.0, 0.0);
    player.placeOnGround(player.getBounds().y);

    for (int index = 0; index < 40; ++index) {
        player.setMoveDirection(1);
        player.update(kStepSeconds);
        player.placeOnGround(player.getBounds().y);
    }

    player.setMoveDirection(-1);
    assert(isPlayerSkidding(player));

    int steps = 0;
    while (isPlayerSkidding(player) && steps < 100) {
        player.update(kStepSeconds);
        player.placeOnGround(player.getBounds().y);
        ++steps;
    }
    assert(!isPlayerSkidding(player));
    assert(steps < 40);

    assert(selectPlayerAnimationState(player, false, false, true) ==
           PlayerAnimationState::Skid);
    assert(selectPlayerAnimationState(player, false, false, false) ==
           PlayerAnimationState::Run);

    player.jump();
    assert(!player.isOnGround());
    assert(selectPlayerAnimationState(player, false, false, true) ==
           PlayerAnimationState::Jump);
}

void testThrowStateOutranksLocomotion() {
    Player player(0.0, 0.0);
    player.upgradeToFire();
    player.placeOnGround(player.getBounds().y);
    player.setMoveDirection(1);
    player.update(kStepSeconds);

    assert(player.shootFireball() != nullptr);
    assert(player.isThrowing());
    assert(selectPlayerAnimationState(player, false, false) ==
           PlayerAnimationState::Throw);

    for (int index = 0; index < 100 && player.isThrowing(); ++index) {
        player.update(kStepSeconds);
    }
    assert(!player.isThrowing());
    assert(selectPlayerAnimationState(player, false, false) !=
           PlayerAnimationState::Throw);
}

void testLandingEventFiresExactlyOnce() {
    Player player(200.0, 0.0);
    const double groundColliderY = 400.0;

    bool wasOnGround = player.isOnGround();
    int landings = 0;
    double impactAtLanding = 0.0;

    for (int index = 0; index < 200; ++index) {
        player.update(kStepSeconds);
        player.moveY(kStepSeconds);
        if (player.getBounds().y >= groundColliderY) {
            player.placeOnGround(groundColliderY);
        }

        if (!wasOnGround && player.isOnGround()) {
            ++landings;
            impactAtLanding = player.getLandingImpactSpeed();
        }
        wasOnGround = player.isOnGround();
    }

    assert(landings == 1);
    assert(impactAtLanding > 300.0);
    assert(player.getLandingImpactSpeed() == impactAtLanding);
}

void testPlayerDeathSequenceDelaysRespawn() {
    World world;
    Player& player = world.getPlayer();

    player.takeDamage();
    assert(!player.isAlive());
    assert(!player.isDeathAnimationFinished());

    world.update(kStepSeconds);
    assert(world.getLives() == 3);
    assert(!player.isAlive());

    int steps = 1;
    while (world.getLives() == 3 && steps < 400) {
        world.update(kStepSeconds);
        ++steps;
    }

    assert(world.getLives() == 2);
    assert(world.getPlayer().isAlive());
    assert(steps > 50);
    assert(world.getTimeRemaining() == 600);
}

void testGoombaSquishLingersThenIsRemoved() {
    Player player(0.0, 0.0);
    Goomba goomba(100.0, 100.0);

    goomba.onStomped(player);
    assert(!goomba.isAlive());
    assert(!goomba.isRemovable());

    const double squishedX = goomba.getX();
    for (int index = 0; index < 20; ++index) {
        goomba.update(kStepSeconds);
        goomba.moveX(kStepSeconds);
    }
    assert(goomba.getX() == squishedX);
    assert(!goomba.isRemovable());

    for (int index = 0; index < 60 && !goomba.isRemovable(); ++index) {
        goomba.update(kStepSeconds);
    }
    assert(goomba.isRemovable());
}

void testKoopaWalkShellSlideTransitions() {
    Player player(0.0, 0.0);
    Koopa koopa(100.0, 100.0, KoopaColor::Green);

    assert(koopa.isAlive());
    assert(!koopa.isShell());
    const double walkBottom = koopa.getY() + koopa.getHeight();

    koopa.onStomped(player);
    assert(koopa.isShell());
    assert(!koopa.isSlidingShell());
    assert(koopa.getHeight() == Koopa::kShellHeight);
    assert(koopa.getY() + koopa.getHeight() == walkBottom);

    koopa.onStomped(player);
    assert(koopa.isSlidingShell());
    assert(koopa.isDeadlyToEnemies());
}

void testEffectManagerDropsFinishedEffects() {
    EffectManager effects;

    effects.spawnSmoke(10.0, 20.0);
    const std::size_t puffsPerBurst = effects.activeCount();
    assert(puffsPerBurst >= 2 && puffsPerBurst <= 3);

    effects.spawnImpact(30.0, 40.0);
    assert(effects.activeCount() == puffsPerBurst + 1);

    effects.update(kFrameMs);
    assert(effects.activeCount() == puffsPerBurst + 1);

    std::size_t distinctCounts = 0;
    std::size_t previousCount = effects.activeCount();
    for (int index = 0; index < 60 && effects.activeCount() > 0; ++index) {
        effects.update(kFrameMs);
        if (effects.activeCount() != previousCount) {
            ++distinctCounts;
            previousCount = effects.activeCount();
        }
    }
    assert(effects.activeCount() == 0);
    assert(distinctCounts >= 3);

    effects.spawnSmoke(0.0, 0.0);
    effects.clear();
    assert(effects.activeCount() == 0);
}
}

int main() {
    testLoopAnimationKeepsCycling();
    testTransientAnimationFinishesOnLastFrame();
    testSingleFrameTransientFinishes();
    testAirborneSplitsJumpAndFall();
    testGroundStatesAndPriority();
    testTurnLatchOutlivesSkidCondition();
    testThrowStateOutranksLocomotion();
    testLandingEventFiresExactlyOnce();
    testPlayerDeathSequenceDelaysRespawn();
    testGoombaSquishLingersThenIsRemoved();
    testKoopaWalkShellSlideTransitions();
    testEffectManagerDropsFinishedEffects();

    std::cout << "Animation and effect lifecycle passed\n";
    return 0;
}
