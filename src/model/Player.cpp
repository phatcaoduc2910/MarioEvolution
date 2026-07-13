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
     * Starts a short invincibility window after the player takes damage.
     *
     * @param player Player instance that should receive temporary invincibility.
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
 * Makes the player jump when the player is alive and not already moving
 * vertically.
 *
 * The current model does not expose an onGround flag yet, so near-zero vertical
 * velocity is used as the temporary jump guard.
 */
void Player::jump() {
    if (alive && state != PlayerState::Dead && std::abs(velocityY) < kMinVelocity) {
        velocityY = kJumpVelocity;
    }
}

/**
 * Applies an item effect to the player and marks the item as collected.
 *
 * @param item Item touched by the player.
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
 * Applies damage to the player according to the current state.
 *
 * FIRE downgrades to BIG, BIG downgrades to SMALL, and SMALL changes to DEAD.
 * Downgrade states grant a short invincibility period.
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
 * Handles reaching the level flag.
 *
 * The player stops moving, snaps horizontally to the flag, and delegates the
 * completion state to the flag object.
 *
 * @param flag Flag touched by the player.
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
 * Fires a projectile when the fire power-up system is available.
 *
 * This is intentionally empty until the project defines a Fireball entity and
 * projectile manager.
 */
void Player::shootFireball() {
    //@todo: update Fireball.cpp to code this function
}

/**
 * Updates the player physics and temporary invincibility timer.
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
 * Renders the player.
 *
 * Rendering is currently handled by the view layer, so the model render hook is
 * intentionally empty.
 */
void Player::render() {}
