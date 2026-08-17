#include "model/Player.h"
#include "model/Flag.h"
#include "model/Item.h"

#include <SDL2/SDL.h>

namespace {
constexpr double kJumpVelocityPixelsPerSecond = -600.0;
constexpr double kStompBounceVelocityPixelsPerSecond = -360.0;
constexpr double kMoveSpeedPixelsPerSecond = 240.0;
constexpr double kInvincibilityDurationSeconds = 1.5;
constexpr double kTimerEpsilonSeconds = 1e-9;
}

Player::Player(double x, double y)
    : Actor(x, y, 32, 48),
      state(PlayerState::Small),
      invincibilityRemainingSeconds(0.0) {
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
    return invincibilityRemainingSeconds > 0.0;
}

void Player::jump() {
    // A1: Trạng thái chạm đất, không phải vận tốc dọc, quyết định quyền nhảy.
    if (!isAlive() || !isOnGround()) {
        return;
    }

    velocityY = kJumpVelocityPixelsPerSecond;
    onGround = false;
}

void Player::bounceAfterStomp() {
    // A4: Cú stomp tạo lực bật ngắn hơn một lần nhảy thường.
    if (!isAlive()) {
        return;
    }

    velocityY = kStompBounceVelocityPixelsPerSecond;
    onGround = false;
}

void Player::setMoveDirection(int direction) {
    if (!isAlive()) {
        velocityX = 0.0;
        return;
    }

    if (direction < 0) {
        velocityX = -kMoveSpeedPixelsPerSecond;
    } else if (direction > 0) {
        velocityX = kMoveSpeedPixelsPerSecond;
    } else {
        velocityX = 0.0;
    }
}
void Player::collect(Item& item) {
    if (!isAlive()) { return; }

    item.applyTo(*this);
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
        invincibilityRemainingSeconds = 0.0;
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

void Player::update(double dtSeconds) {
    if (!isAlive()) {
        return;
    }

    if (invincibilityRemainingSeconds > 0.0) {
        if (dtSeconds >=
            invincibilityRemainingSeconds - kTimerEpsilonSeconds) {
            invincibilityRemainingSeconds = 0.0;
        } else {
            invincibilityRemainingSeconds -= dtSeconds;
        }
    }

    // Chỉ dựng ý định di chuyển; CollisionSystem mới dời vị trí theo từng trục.
    applyGravity(dtSeconds);
}

void Player::startInvincibility() {
    invincibilityRemainingSeconds = kInvincibilityDurationSeconds;
}

void Player::render() {}
