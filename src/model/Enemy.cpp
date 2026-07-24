#include "model/Enemy.h"

#include "model/Player.h"

#include <cmath>

namespace {
constexpr double kShellKickSpeed = 5.0;
constexpr double kStillVelocity = 0.001;
}

Enemy::Enemy(double x, double y, int width, int height)
    : Actor(x, y, width, height),
      walkingSpeed(1),
      state(EnemyState::Walking) {}

void Enemy::patrol() {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    velocityX = (direction == Direction::Left) ? -walkingSpeed : walkingSpeed;
    move();
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

void Goomba::patrol() {
    Enemy::patrol();
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

void Koopa::kick() {
    if (!alive) {
        return;
    }

    shellMode = true;
    state = EnemyState::Shell;
    velocityX = (direction == Direction::Left) ? -kShellKickSpeed : kShellKickSpeed;
    move();
}

void Koopa::patrol() {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    if (shellMode) {
        if (std::abs(velocityX) > kStillVelocity) {
            move();
        }
        return;
    }

    Enemy::patrol();
}
