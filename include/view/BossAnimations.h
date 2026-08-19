#pragma once

#include "SpriteAnimation.h"
#include "core/Types.h"

// Bảng frame khớp assets/enemies/boss_gorilla/, khoá theo BossAction của model.
// Action một lần (không lặp) dùng isFinished() để biết lúc nào hành động xong.
inline SpriteAnimation makeBossAnimation(BossAction action) {
    switch (action) {
        case BossAction::WindUpThrow:
            return SpriteAnimation(
                {"boss.throw.windup.1", "boss.throw.windup.2"}, 140, false);
        case BossAction::Throw:
            return SpriteAnimation({"boss.throw.1", "boss.throw.2"}, 90, false);
        case BossAction::Recover:
            return SpriteAnimation(
                {"boss.recover.1", "boss.recover.2"}, 130, false);
        case BossAction::Charge:
            return SpriteAnimation({"boss.charge.1", "boss.charge.2",
                                    "boss.charge.3", "boss.charge.4"},
                                   110);
        case BossAction::GroundSlam:
            return SpriteAnimation({"boss.slam.1", "boss.slam.2", "boss.slam.3",
                                    "boss.slam.4", "boss.slam.5"},
                                   120, false);
        case BossAction::Hurt:
            return SpriteAnimation({"boss.hurt.1", "boss.hurt.2"}, 110, false);
        case BossAction::Enraged:
            return SpriteAnimation({"boss.enraged.1", "boss.enraged.2",
                                    "boss.enraged.3", "boss.enraged.4"},
                                   150, false);
        case BossAction::Dodge:
            return SpriteAnimation({"boss.dodge.1", "boss.dodge.2",
                                    "boss.dodge.3", "boss.dodge.4",
                                    "boss.dodge.5", "boss.dodge.6"},
                                   80, false);
        case BossAction::Death:
            return SpriteAnimation(
                {"boss.death.1", "boss.death.2", "boss.death.3"}, 260, false);
        case BossAction::Idle:
        default:
            return SpriteAnimation(
                {"boss.idle.1", "boss.idle.2", "boss.idle.3"}, 220);
    }
}
