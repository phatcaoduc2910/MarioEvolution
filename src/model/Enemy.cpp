#include "model/Enemy.h"

#include "model/Player.h"

Enemy::Enemy(double x, double y, int width, int height)
    : Actor(x, y, width, height),
      walkingSpeed(1),
      state(EnemyState::Walking) {}

void Enemy::patrol() {}

void Enemy::die() {}

void Enemy::damagePlayer(Player& player) {
    (void)player;
}

Goomba::Goomba(double x, double y)
    : Enemy(x, y, 32, 32) {}

void Goomba::patrol() {}

void Goomba::die() {}

Koopa::Koopa(double x, double y)
    : Enemy(x, y, 32, 48),
      shellMode(false) {}

void Koopa::hideInShell() {}

void Koopa::kick() {}

void Koopa::patrol() {}
