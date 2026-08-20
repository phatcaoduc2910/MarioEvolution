#include "model/Player.h"
#include "model/Fireball.h"
#include "model/Flag.h"
#include "model/Item.h"

#include <SDL2/SDL.h>
#include <algorithm>

namespace {
constexpr double kJumpVelocityPixelsPerSecond = -600.0;
constexpr double kStompBounceVelocityPixelsPerSecond = -360.0;
constexpr double kMoveSpeedPixelsPerSecond = 240.0;
constexpr double kAccelerationPixelsPerSecondSquared = 1200.0;
constexpr double kDecelerationPixelsPerSecondSquared = 800.0;
constexpr double kInvincibilityDurationSeconds = 1.5;
constexpr double kTimerEpsilonSeconds = 1e-9;
constexpr double kDeathHopVelocityPixelsPerSecond = -420.0;
constexpr double kDeathSequenceSeconds = 1.2;
constexpr double kThrowAnimationSeconds = 0.24;

double tickDown(double remainingSeconds, double dtSeconds) {
    if (remainingSeconds <= 0.0) {
        return 0.0;
    }
    if (dtSeconds >= remainingSeconds - kTimerEpsilonSeconds) {
        return 0.0;
    }
    return remainingSeconds - dtSeconds;
}
}

Player::Player(double x, double y)
    : Actor(x, y, kBodyWidth, kSmallHeight),
      state(PlayerState::Small),
      moveDirection(0),
      invincibilityRemainingSeconds(0.0),
      deathElapsedSeconds(0.0),
      throwRemainingSeconds(0.0) {
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

bool Player::isDeathAnimationFinished() const {
    return !isAlive() && deathElapsedSeconds >= kDeathSequenceSeconds;
}

bool Player::isThrowing() const {
    return throwRemainingSeconds > 0.0;
}

int Player::getMoveDirection() const {
    return moveDirection;
}

Rectangle Player::getBounds() const {
    return {
        x + kColliderInsetX,
        y + kColliderInsetTop,
        kColliderWidth,
        height - kColliderInsetTop
    };
}

Rectangle Player::getFeetBounds() const {
    const Rectangle body = getBounds();

    return {
        body.x + kFeetInsetX,
        body.y + body.height - kFeetHeight,
        body.width - 2 * kFeetInsetX,
        kFeetHeight
    };
}


void Player::jump() {
    // Xử lý lỗi nhảy 2 lần trên không
    if (!isAlive() || !isOnGround()) {
        return;
    }

    velocityY = kJumpVelocityPixelsPerSecond;
    onGround = false;
}

void Player::bounceAfterStomp() {
    if (!isAlive()) {
        return;
    }

    velocityY = kStompBounceVelocityPixelsPerSecond;
    onGround = false;
}

void Player::setMoveDirection(int direction) {
    if (!isAlive()) {
        moveDirection = 0;
        velocityX = 0.0;
        return;
    }
    // Quy ước hướng di chuyển là -1 0 1 
    moveDirection = std::clamp(direction, -1, 1);
}
void Player::collect(Item& item) {
    if (!isAlive()) { return; }

    item.applyTo(*this);
}

void Player::grow() {
    if (state == PlayerState::Small) {
        state = PlayerState::Big;
        resizeForState();
    }
}

void Player::upgradeToFire() {
    if (isAlive()) {
        state = PlayerState::Fire;
        resizeForState();
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
        resizeForState();
        startInvincibility();
    } else {
        state = PlayerState::Dead;
        invincibilityRemainingSeconds = 0.0;
        throwRemainingSeconds = 0.0;
        moveDirection = 0;
        velocityX = 0.0;
        velocityY = kDeathHopVelocityPixelsPerSecond;
        deathElapsedSeconds = 0.0;
        onGround = false;
        return;
    }

    throwRemainingSeconds = 0.0;
}

// Hồi sinh người chơi tại vị trí an toàn.
void Player::reviveAt(double reviveX, double reviveY) {
    x = reviveX;
    y = reviveY;
    state = PlayerState::Small;
    height = kSmallHeight;
    moveDirection = 0;
    velocityX = 0.0;
    velocityY = 0.0;
    onGround = false;
    deathElapsedSeconds = 0.0;
    throwRemainingSeconds = 0.0;
    startInvincibility();
}

void Player::captureFlag(Flag& flag) {
    if (!isAlive()) {
        return;
    }

    velocityX = 0.0;
    velocityY = 0.0;
    moveDirection = 0;
    x = flag.getX();

    flag.onCapture(*this);
}


std::unique_ptr<Fireball> Player::shootFireball() {
    if (!isAlive() || state != PlayerState::Fire) {
        return nullptr;
    }
    // Giới hạn thời gian ném fireball  
    throwRemainingSeconds = kThrowAnimationSeconds;

    const double fireballX = direction == Direction::Left
                                 ? x - Fireball::kSize
                                 : x + width;
    const double fireballY = y + (height - Fireball::kSize) / 2.0;
    return std::make_unique<Fireball>(fireballX, fireballY, direction);
}

void Player::update(double dtSeconds) {
    if (!isAlive()) {
        deathElapsedSeconds += dtSeconds;
        velocityX = 0.0;
        applyGravity(dtSeconds);
        return;
    }

    invincibilityRemainingSeconds =
        tickDown(invincibilityRemainingSeconds, dtSeconds);
    throwRemainingSeconds = tickDown(throwRemainingSeconds, dtSeconds);

    // Đổi sang di chuyển có gia tốc thay vì đặt vận tốc trực tiếp
    const double targetVelocityX =
        moveDirection * kMoveSpeedPixelsPerSecond;
    const double changeRate = moveDirection == 0
                                  ? kDecelerationPixelsPerSecondSquared
                                  : kAccelerationPixelsPerSecondSquared;
    const double maxChange = changeRate * dtSeconds;
    velocityX += std::clamp(
        targetVelocityX - velocityX, -maxChange, maxChange);

    applyGravity(dtSeconds);
}

void Player::startInvincibility() {
    invincibilityRemainingSeconds = kInvincibilityDurationSeconds;
}

void Player::resizeForState() {
    if (state == PlayerState::Dead) {
        return;
    }

    const int targetHeight = (state == PlayerState::Small)
                                 ? kSmallHeight
                                 : kBigHeight;
    if (targetHeight == height) {
        return;
    }

    y += height - targetHeight;
    height = targetHeight;
}

void Player::render() {}
