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
 * Di chuyển kẻ địch theo hướng hiện tại.
 *
 * Việc đổi hướng do tường hoặc mép nền nên được điều phối bởi hệ thống va chạm
 * hoặc hệ thống màn chơi.
 */
void Enemy::patrol() {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    velocityX = (direction == Direction::Left) ? -walkingSpeed : walkingSpeed;
    move();
}

/**
 * Đánh dấu kẻ địch đã chết và dừng toàn bộ chuyển động.
 */
void Enemy::die() {
    state = EnemyState::Dead;
    alive = false;
    velocityX = 0.0;
    velocityY = 0.0;
}

/**
 * Gây sát thương cho người chơi khi kẻ địch này đang hoạt động.
 *
 * @param player Người chơi va chạm với kẻ địch.
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
 * Dùng hành vi tuần tra cơ bản của kẻ địch cho Goomba.
 */
void Goomba::patrol() {
    Enemy::patrol();
}

/**
 * Dùng hành vi chết cơ bản của kẻ địch cho Goomba.
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
 * Chuyển Koopa sang chế độ mai rùa mà không di chuyển ngang.
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
 * Đá mai rùa của Koopa theo hướng hiện tại.
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
 * Cập nhật hành vi tuần tra của Koopa.
 *
 * Ở chế độ thường, Koopa đi như kẻ địch thông thường. Ở chế độ mai rùa, Koopa
 * chỉ di chuyển khi mai có vận tốc ngang khác 0.
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
