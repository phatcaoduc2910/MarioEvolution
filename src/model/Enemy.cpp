#include "model/Enemy.h"

#include "model/Player.h"

#include <cmath>

namespace {
constexpr double kWalkingSpeedPixelsPerSecond = 60.0;
constexpr double kShellKickSpeedPixelsPerSecond = 300.0;
constexpr double kStillVelocityPixelsPerSecond = 0.06;
}

Enemy::Enemy(double x, double y, int width, int height)
    : Actor(x, y, width, height),
      walkingSpeed(kWalkingSpeedPixelsPerSecond),
      state(EnemyState::Walking) {}

void Enemy::patrol(double dtSeconds) {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    velocityX = (direction == Direction::Left) ? -walkingSpeed : walkingSpeed;
    move(dtSeconds);
}

void Enemy::die() {
    state = EnemyState::Dead;
    alive = false;
    velocityX = 0.0;
    velocityY = 0.0;
}

void Enemy::damagePlayer(Player& player) {
    if (alive && state != EnemyState::Dead) {
        player.takeDamage();
    }
}

Goomba::Goomba(double x, double y)
    : Enemy(x, y, 32, 32) {
    direction = Direction::Left;
}

void Goomba::patrol(double dtSeconds) {
    Enemy::patrol(dtSeconds);
}

void Goomba::die() {
    Enemy::die();
}

Koopa::Koopa(double x, double y)
    : Enemy(x, y, 32, 48),
      shellMode(false) {
    direction = Direction::Left;
}

void Koopa::hideInShell() {
    if (!alive) {
        return;
    }

    shellMode = true;
    state = EnemyState::Shell;
    velocityX = 0.0;
}

void Koopa::kick(double dtSeconds) {
    if (!alive) {
        return;
    }

    shellMode = true;
    state = EnemyState::Shell;
    velocityX = (direction == Direction::Left)
                    ? -kShellKickSpeedPixelsPerSecond
                    : kShellKickSpeedPixelsPerSecond;
    move(dtSeconds);
}

void Koopa::patrol(double dtSeconds) {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    if (shellMode) {
        if (std::abs(velocityX) > kStillVelocityPixelsPerSecond) {
            move(dtSeconds);
        }
        return;
    }

    Enemy::patrol(dtSeconds);
}
