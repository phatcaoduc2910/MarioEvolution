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

/**
 * Moves the enemy in its current facing direction.
 *
 * Direction changes caused by walls or platform edges should be coordinated by
 * the collision or level system.
 */
void Enemy::patrol() {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    velocityX = (direction == Direction::Left) ? -walkingSpeed : walkingSpeed;
    move();
}

/**
 * Marks the enemy as dead and stops all movement.
 */
void Enemy::die() {
    state = EnemyState::Dead;
    alive = false;
    velocityX = 0.0;
    velocityY = 0.0;
}

/**
 * Damages the player when this enemy is active.
 *
 * @param player Player that collided with the enemy.
 */
void Enemy::damagePlayer(Player& player) {
    if (alive && state != EnemyState::Dead) {
        player.takeDamage();
    }
}

Goomba::Goomba(double x, double y)
    : Enemy(x, y, 32, 32) {
    direction = Direction::Left;
}

/**
 * Uses the base enemy patrol behavior for Goomba.
 */
void Goomba::patrol() {
    Enemy::patrol();
}

/**
 * Uses the base enemy death behavior for Goomba.
 */
void Goomba::die() {
    Enemy::die();
}

Koopa::Koopa(double x, double y)
    : Enemy(x, y, 32, 48),
      shellMode(false) {
    direction = Direction::Left;
}

/**
 * Switches Koopa into shell mode without horizontal movement.
 */
void Koopa::hideInShell() {
    if (!alive) {
        return;
    }

    shellMode = true;
    state = EnemyState::Shell;
    velocityX = 0.0;
}

/**
 * Kicks Koopa's shell in its current facing direction.
 */
void Koopa::kick() {
    if (!alive) {
        return;
    }

    shellMode = true;
    state = EnemyState::Shell;
    velocityX = (direction == Direction::Left) ? -kShellKickSpeed : kShellKickSpeed;
    move();
}

/**
 * Updates Koopa patrol behavior.
 *
 * In normal mode Koopa walks like a regular enemy. In shell mode it only moves
 * while the shell has a non-zero horizontal velocity.
 */
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
