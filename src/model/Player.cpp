#include "model/Player.h"
#include "model/Flag.h"
#include "model/Item.h"

#include <SDL2/SDL.h>
#include <cmath>
#include <unordered_map>

namespace {
    constexpr double kJumpVelocity = -10.0;
    constexpr double kMinVelocity = 0.001;
    constexpr int kInvincibleFrames = 90;

    std::unordered_map<const Player*, int> invincibleTimers;

    /**
     * Bắt đầu một khoảng bất tử ngắn sau khi người chơi nhận sát thương.
     *
     * @param player Instance người chơi cần nhận bất tử tạm thời.
     */
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

/**
 * Cho người chơi nhảy khi còn sống và chưa di chuyển theo chiều dọc.
 *
 * Model hiện tại chưa có cờ onGround, nên vận tốc dọc gần bằng 0 được dùng làm
 * điều kiện chặn nhảy tạm thời.
 */
void Player::jump() {
    if (alive && state != PlayerState::Dead && std::abs(velocityY) < kMinVelocity) {
        velocityY = kJumpVelocity;
    }
}

/**
 * Áp dụng hiệu ứng item cho người chơi và đánh dấu item đã được nhặt.
 *
 * @param item Item mà người chơi chạm vào.
 */
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

/**
 * Gây sát thương cho người chơi theo trạng thái hiện tại.
 *
 * FIRE giảm xuống BIG, BIG giảm xuống SMALL, còn SMALL chuyển thành DEAD.
 * Các lần giảm cấp sẽ cấp một khoảng bất tử ngắn.
 */
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

/**
 * Xử lý khi người chơi chạm cờ cuối màn.
 *
 * Người chơi dừng di chuyển, căn ngang vào cờ và giao trạng thái hoàn thành cho
 * object cờ.
 *
 * @param flag Cờ mà người chơi chạm vào.
 */
void Player::captureFlag(Flag& flag) {
    if (!alive || state == PlayerState::Dead) {
        return;
    }

    velocityX = 0.0;
    velocityY = 0.0;
    x = flag.getX();

    flag.onCapture(*this);
}

/**
 * Bắn đạn khi hệ thống power-up lửa sẵn sàng.
 *
 * Hàm này cố ý để trống cho đến khi project định nghĩa entity Fireball và bộ
 * quản lý đạn.
 */
void Player::shootFireball() {
    //@todo: cập nhật Fireball.cpp để viết hàm này
}

/**
 * Cập nhật vật lý người chơi và bộ đếm bất tử tạm thời.
 */
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

/**
 * Render người chơi.
 *
 * Việc render hiện do tầng view xử lý, nên hook render trong model được cố ý để
 * trống.
 */
void Player::render() {}
