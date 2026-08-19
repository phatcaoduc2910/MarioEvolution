#include "model/Enemy.h"

#include "model/Player.h"

namespace {
constexpr double kWalkingSpeedPixelsPerSecond = 60.0;
constexpr double kShellSlideSpeedPixelsPerSecond = 300.0;

constexpr double kPiranhaHiddenSeconds = 1.8;
constexpr double kPiranhaRisingSeconds = 0.45;
constexpr double kPiranhaExposedSeconds = 1.6;
constexpr double kPiranhaSinkingSeconds = 0.45;
constexpr double kEnemyDeathDisplaySeconds = 0.5;

Direction awayFromPlayer(const Player& player, const GameObject& enemy) {
    const double playerCenterX = player.getX() + player.getWidth() / 2.0;
    const double enemyCenterX = enemy.getX() + enemy.getWidth() / 2.0;

    return (playerCenterX <= enemyCenterX) ? Direction::Right : Direction::Left;
}

double phaseDurationSeconds(PiranhaPhase phase) {
    switch (phase) {
        case PiranhaPhase::Hidden:
            return kPiranhaHiddenSeconds;
        case PiranhaPhase::Rising:
            return kPiranhaRisingSeconds;
        case PiranhaPhase::Exposed:
            return kPiranhaExposedSeconds;
        case PiranhaPhase::Sinking:
        default:
            return kPiranhaSinkingSeconds;
    }
}

PiranhaPhase nextPhase(PiranhaPhase phase) {
    switch (phase) {
        case PiranhaPhase::Hidden:
            return PiranhaPhase::Rising;
        case PiranhaPhase::Rising:
            return PiranhaPhase::Exposed;
        case PiranhaPhase::Exposed:
            return PiranhaPhase::Sinking;
        case PiranhaPhase::Sinking:
        default:
            return PiranhaPhase::Hidden;
    }
}

double risenRatio(PiranhaPhase phase, double phaseElapsedSeconds) {
    switch (phase) {
        case PiranhaPhase::Hidden:
            return 0.0;
        case PiranhaPhase::Rising:
            return phaseElapsedSeconds / kPiranhaRisingSeconds;
        case PiranhaPhase::Exposed:
            return 1.0;
        case PiranhaPhase::Sinking:
        default:
            return 1.0 - phaseElapsedSeconds / kPiranhaSinkingSeconds;
    }
}
}

Enemy::Enemy(double x, double y, int width, int height)
    : Actor(x, y, width, height),
      walkingSpeed(kWalkingSpeedPixelsPerSecond),
      state(EnemyState::Walking),
      deathElapsedSeconds(0.0) {}

void Enemy::patrol() {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    velocityX = (direction == Direction::Left) ? -walkingSpeed : walkingSpeed;
}

void Enemy::reverseDirection() {
    direction = (direction == Direction::Left) ? Direction::Right : Direction::Left;
}

bool Enemy::shouldTurnAtEdge() const {
    return true;
}

bool Enemy::isStompable() const {
    return true;
}

void Enemy::onStomped(Player& player) {
    (void)player;
    die();
}

bool Enemy::isDeadlyToEnemies() const {
    return false;
}

bool Enemy::isRemovable() const {
    return !alive && deathElapsedSeconds >= kEnemyDeathDisplaySeconds;
}

void Enemy::tickDeath(double dtSeconds) {
    deathElapsedSeconds += dtSeconds;
}

void Enemy::update(double dtSeconds) {
    if (!alive) {
        tickDeath(dtSeconds);
        return;
    }

    applyGravity(dtSeconds);
    patrol();
}

void Enemy::die() {
    if (!alive) {
        return;
    }

    state = EnemyState::Dead;
    alive = false;
    velocityX = 0.0;
    velocityY = 0.0;
    deathElapsedSeconds = 0.0;
}

void Enemy::onPlayerContact(Player& player) {
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

Koopa::Koopa(double x, double y, KoopaColor color)
    : Enemy(x, y, kWalkWidth, kWalkHeight),
      color(color) {
    direction = Direction::Left;
}

KoopaColor Koopa::getColor() const {
    return color;
}

bool Koopa::isShell() const {
    return state == EnemyState::Shell;
}

bool Koopa::isSlidingShell() const {
    return state == EnemyState::ShellSliding;
}

void Koopa::hideInShell() {
    if (!alive) {
        return;
    }

    if (height > kShellHeight) {
        y += height - kShellHeight;
        height = kShellHeight;
    }

    state = EnemyState::Shell;
    velocityX = 0.0;
}

void Koopa::kick(Direction slideDirection) {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    direction = slideDirection;
    state = EnemyState::ShellSliding;
}

void Koopa::patrol() {
    if (!alive || state == EnemyState::Dead) {
        return;
    }

    if (state == EnemyState::Shell) {
        velocityX = 0.0;
        return;
    }

    const double speed = (state == EnemyState::ShellSliding)
                             ? kShellSlideSpeedPixelsPerSecond
                             : walkingSpeed;
    velocityX = (direction == Direction::Left) ? -speed : speed;
}

void Koopa::onPlayerContact(Player& player) {
    if (state == EnemyState::Shell) {
        kick(awayFromPlayer(player, *this));
        return;
    }

    Enemy::onPlayerContact(player);
}

void Koopa::onStomped(Player& player) {
    if (state == EnemyState::Shell) {
        kick(awayFromPlayer(player, *this));
        return;
    }

    hideInShell();
}

bool Koopa::shouldTurnAtEdge() const {
    return color == KoopaColor::Red && state == EnemyState::Walking;
}

bool Koopa::isDeadlyToEnemies() const {
    return alive && state == EnemyState::ShellSliding;
}

PiranhaPlant::PiranhaPlant(double x, double mouthY)
    : Enemy(x, mouthY, kPlantWidth, 0),
      phase(PiranhaPhase::Hidden),
      phaseElapsedSeconds(0.0),
      mouthY(mouthY) {}

PiranhaPhase PiranhaPlant::getPhase() const {
    return phase;
}

void PiranhaPlant::update(double dtSeconds) {
    if (!alive) {
        tickDeath(dtSeconds);
        return;
    }

    phaseElapsedSeconds += dtSeconds;
    while (phaseElapsedSeconds >= phaseDurationSeconds(phase)) {
        phaseElapsedSeconds -= phaseDurationSeconds(phase);
        phase = nextPhase(phase);
    }

    applyRisenHeight();
}

void PiranhaPlant::applyGravity(double) {}

void PiranhaPlant::patrol() {}

void PiranhaPlant::onPlayerContact(Player& player) {
    if (phase == PiranhaPhase::Hidden) {
        return;
    }

    Enemy::onPlayerContact(player);
}

bool PiranhaPlant::isStompable() const {
    return false;
}

bool PiranhaPlant::shouldTurnAtEdge() const {
    return false;
}

void PiranhaPlant::applyRisenHeight() {
    const double ratio = risenRatio(phase, phaseElapsedSeconds);
    height = static_cast<int>(kPlantHeight * ratio);
    y = mouthY - height;
}
