#include "model/Enemy.h"

#include "model/Player.h"

namespace {
constexpr double kWalkingSpeedPixelsPerSecond = 60.0;
constexpr double kShellKickSpeedPixelsPerSecond = 300.0;
}

Enemy::Enemy(double x, double y, int width, int height)
    : Actor(x, y, width, height),
      walkingSpeed(kWalkingSpeedPixelsPerSecond),
      state(EnemyState::Walking) {}

void Enemy::patrol() {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    velocityX = (direction == Direction::Left) ? -walkingSpeed : walkingSpeed;
}

void Enemy::reverseDirection() {
    direction = (direction == Direction::Left) ? Direction::Right : Direction::Left;
}

void Enemy::update(double dtSeconds) {
    if (!alive) {
        return;
    }

    applyGravity(dtSeconds);
    patrol();
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
    velocityX = (direction == Direction::Left)
                    ? -kShellKickSpeedPixelsPerSecond
                    : kShellKickSpeedPixelsPerSecond;
}

void Koopa::patrol() {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    // Shell giữ nguyên velocityX của cú kick, không ép về tốc độ đi bộ.
    if (shellMode) {
        return;
    }

    Enemy::patrol();
}
