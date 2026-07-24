#include "model/Player.h"
#include "model/Flag.h"
#include "model/Item.h"

#include <SDL2/SDL.h>
#include <cmath>
#include <unordered_map>

namespace {
    constexpr double kJumpVelocity = -10.0;
    constexpr double kMoveSpeed = 4.0;
    constexpr double kMinVelocity = 0.001;
    constexpr int kInvincibleFrames = 90;

    std::unordered_map<const Player*, int> invincibleTimers;
    void startInvincibility(Player* player) {
        invincibleTimers[player] = kInvincibleFrames;
    }
}
Player::Player(double x, double y)
    : Actor(x, y, 32, 48),
    powerUp(PowerUpType::None),
    state(PlayerState::Small),
    invincible(false) {
    SDL_Log("Player created at x=%.2f, y=%.2f", x, y);
}

PowerUpType Player::getPowerUp() const {
    return powerUp;
}

PlayerState Player::getState() const {
    return state;
}

bool Player::isInvincible() const {
    return invincible;
}

void Player::jump() {
    if (alive && state != PlayerState::Dead && std::abs(velocityY) < kMinVelocity) {
        velocityY = kJumpVelocity;
    }
}

void Player::setMoveDirection(int direction) {
    if (!alive || state == PlayerState::Dead) {
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
    if (!alive || state == PlayerState::Dead || item.isCollected()) { return; }

    item.applyTo(*this);

    if (dynamic_cast<FireFlower*>(&item) != nullptr) {
        powerUp = PowerUpType::FireFlower;
        state = PlayerState::Fire;
    } else if (dynamic_cast<Mushroom*>(&item) != nullptr) {
        if (state != PlayerState::Fire) {
            powerUp = PowerUpType::Mushroom;
            state = PlayerState::Big;
        }
    }
    item.collect();
}

void Player::takeDamage() {
    if (!alive || state == PlayerState::Dead || invincible) {
        return;
    }

    if (state == PlayerState::Fire) {
        state = PlayerState::Big;
        powerUp = PowerUpType::Mushroom;
        invincible = true;
        startInvincibility(this);
    } else if (state == PlayerState::Big) {
        state = PlayerState::Small;
        powerUp = PowerUpType::None;
        invincible = true;
        startInvincibility(this);
    } else {
        state = PlayerState::Dead;
        powerUp = PowerUpType::None;
        alive = false;
        invincible = false;
        velocityX = 0.0;
        velocityY = 0.0;
        invincibleTimers.erase(this);
    }
}

void Player::captureFlag(Flag& flag) {
    if (!alive || state == PlayerState::Dead) {
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
    if (!alive || state == PlayerState::Dead) {
        return;
    }

    auto timer = invincibleTimers.find(this);
    if (timer != invincibleTimers.end()) {
        --timer->second;

        if (timer -> second <= 0) {
            invincibleTimers.erase(timer);
            invincible = false;
        }
    }

    applyGravity();
    move();
}

void Player::render() {}
