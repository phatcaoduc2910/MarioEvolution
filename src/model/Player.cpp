#include "model/Player.h"
#include "model/Flag.h"
#include "model/Item.h"

#include <SDL2/SDL.h>
#include <cmath>

namespace {
constexpr double kJumpVelocity = -10.0;
constexpr double kMoveSpeed = 4.0;
constexpr double kMinVelocity = 0.001;
constexpr int kInvincibleFrames = 90;
}

Player::Player(double x, double y)
    : Actor(x, y, 32, 48),
      state(PlayerState::Small),
      invincibilityFramesRemaining(0) {
    SDL_Log("Player created at x=%.2f, y=%.2f", x, y);
}

PowerUpType Player::getPowerUp() const {
    switch (state) {
        case PlayerState::Big:
            return PowerUpType::Mushroom;
        case PlayerState::Fire:
            return PowerUpType::FireFlower;
        case PlayerState::Small:
        case PlayerState::Dead:
        default:
            return PowerUpType::None;
    }
}

PlayerState Player::getState() const {
    return state;
}

bool Player::isAlive() const {
    return state != PlayerState::Dead;
}

bool Player::isInvincible() const {
    return invincibilityFramesRemaining > 0;
}

void Player::jump() {
    if (isAlive() && std::abs(velocityY) < kMinVelocity) {
        velocityY = kJumpVelocity;
    }
}

void Player::setMoveDirection(int direction) {
    if (!isAlive()) {
        velocityX = 0.0;
        return;
    }

    if (direction < 0) {
        velocityX = -kMoveSpeed;
    } else if (direction > 0) {
        velocityX = kMoveSpeed;
    } else {
        velocityX = 0.0;
    }
}
void Player::collect(Item& item) {
    if (!isAlive() || item.isCollected()) { return; }

    item.applyTo(*this);

    if (dynamic_cast<FireFlower*>(&item) != nullptr) {
        upgradeToFire();
    } else if (dynamic_cast<Mushroom*>(&item) != nullptr) {
        grow();
    }
    item.collect();
}

void Player::grow() {
    if (state == PlayerState::Small) {
        state = PlayerState::Big;
    }
}

void Player::upgradeToFire() {
    if (isAlive()) {
        state = PlayerState::Fire;
    }
}

void Player::takeDamage() {
    if (!isAlive() || isInvincible()) {
        return;
    }

    if (state == PlayerState::Fire) {
        state = PlayerState::Big;
        startInvincibility();
    } else if (state == PlayerState::Big) {
        state = PlayerState::Small;
        startInvincibility();
    } else {
        state = PlayerState::Dead;
        invincibilityFramesRemaining = 0;
        velocityX = 0.0;
        velocityY = 0.0;
    }
}

void Player::captureFlag(Flag& flag) {
    if (!isAlive()) {
        return;
    }

    velocityX = 0.0;
    velocityY = 0.0;
    x = flag.getX();

    flag.onCapture(*this);
}

void Player::shootFireball() {
    // TODO: nối Fireball vào World rồi mới bật phím bắn.
}

void Player::update() {
    if (!isAlive()) {
        return;
    }

    if (invincibilityFramesRemaining > 0) {
        --invincibilityFramesRemaining;
    }

    applyGravity();
    move();
}

void Player::startInvincibility() {
    invincibilityFramesRemaining = kInvincibleFrames;
}

void Player::render() {}
