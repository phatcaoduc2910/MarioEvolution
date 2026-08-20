#pragma once

#include "model/Player.h"

#include <cmath>

enum class PlayerAnimationState {
    Idle,
    Run,
    Jump,
    Fall,
    Skid,
    Throw,
    Transform,
    Hurt,
    Death
};

inline constexpr double kIdleVelocityEpsilon = 0.002;
inline constexpr double kSkidMinSpeedPixelsPerSecond = 60.0;

inline bool isPlayerSkidding(const Player& player) {
    if (!player.isOnGround() || !player.isAlive()) {
        return false;
    }

    const int intent = player.getMoveDirection();
    const double velocityX = player.getVelocityX();
    return (intent > 0 && velocityX < -kSkidMinSpeedPixelsPerSecond) ||
           (intent < 0 && velocityX > kSkidMinSpeedPixelsPerSecond);
}

inline PlayerAnimationState selectPlayerAnimationState(const Player& player,
                                                       bool transforming,
                                                       bool hurt,
                                                       bool turning = false) {
    if (!player.isAlive()) {
        return PlayerAnimationState::Death;
    }
    if (transforming) {
        return PlayerAnimationState::Transform;
    }
    if (hurt) {
        return PlayerAnimationState::Hurt;
    }
    if (player.isThrowing()) {
        return PlayerAnimationState::Throw;
    }
    if (isPlayerSkidding(player) || (turning && player.isOnGround())) {
        return PlayerAnimationState::Skid;
    }
    if (!player.isOnGround()) {
        return player.getVelocityY() < 0.0 ? PlayerAnimationState::Jump
                                           : PlayerAnimationState::Fall;
    }
    if (std::abs(player.getVelocityX()) > kIdleVelocityEpsilon) {
        return PlayerAnimationState::Run;
    }
    return PlayerAnimationState::Idle;
}
