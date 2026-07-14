#include "model/Fireball.h"

namespace {
    constexpr double kFireballSpeed = 7.0;
    constexpr int kFireballSize = 16;
}

/**
 * Tạo quả cầu lửa tại vị trí chỉ định và phóng theo hướng ban đầu.
 *
 * @param x Tọa độ x ban đầu.
 * @param y Tọa độ y ban đầu.
 * @param dir Hướng bay ban đầu của quả cầu lửa.
 */
Fireball::Fireball(double x, double y, Direction dir)
    :  Actor(x, y, kFireballSize, kFireballSize) {
        direction = dir;
        velocityX = (dir == Direction::Left) ? -kFireballSpeed : kFireballSpeed;
        velocityY = 0.0;
    }

/**
 * Cập nhật chuyển động của quả cầu lửa khi nó còn tồn tại.
 */
void Fireball::update() {
    if (!alive) { return; }
    applyGravity();
    move();
}

/**
 * Hủy quả cầu lửa để nó không tiếp tục được cập nhật.
 */
void Fireball::destroy() {
    alive = false;
    velocityX = 0.0;
    velocityY = 0.0;
}
