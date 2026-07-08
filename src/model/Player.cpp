#include "model/Player.h"

#include "model/Flag.h"
#include "model/Item.h"

Player::Player(double x, double y)
    : Actor(x, y, 32, 48),
      powerUp(PowerUpType::None),
      state(PlayerState::Small),
      invincible(false) {}

PowerUpType Player::getPowerUp() const {
    return powerUp;
}

PlayerState Player::getState() const {
    return state;
}

bool Player::isInvincible() const {
    return invincible;
}

void Player::jump() {}

void Player::collect(Item& item) {
    (void)item;
}

void Player::takeDamage() {}

void Player::captureFlag(Flag& flag) {
    (void)flag;
}

void Player::shootFireball() {}

void Player::update() {}

void Player::render() {}
