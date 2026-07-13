#include "model/Actor.h"

#include <algorithm>

namespace {
constexpr double kGravity = 0.45;
constexpr double kMaxFallSpeed = 12.0;

/**
 * Kiểm tra hai hình chữ nhật song song trục có chồng lên nhau không.
 *
 * @param a Hình chữ nhật thứ nhất.
 * @param b Hình chữ nhật thứ hai.
 * @return true nếu hai hình chữ nhật giao nhau; ngược lại là false.
 */
bool intersects(const Rectangle& a, const Rectangle& b) {
    return a.x < b.x + b.width &&
           a.x + a.width > b.x &&
           a.y < b.y + b.height &&
           a.y + a.height > b.y;
}
}

Actor::Actor(double x, double y, int width, int height)
    : GameObject(x, y, width, height),
      velocityX(0.0),
      velocityY(0.0),
      direction(Direction::Right),
      alive(true) {}

bool Actor::isAlive() const {
    return alive;
}

Direction Actor::getDirection() const {
    return direction;
}

/**
 * Áp dụng vận tốc hiện tại vào vị trí của actor.
 *
 * Vận tốc ngang cũng cập nhật hướng nhìn để hệ thống render và gameplay biết
 * actor đang di chuyển sang trái hay phải.
 */
void Actor::move() {
    x += velocityX;
    y += velocityY;

    if (velocityX < 0.0) {
        direction = Direction::Left;
    } else if (velocityX > 0.0) {
        direction = Direction::Right;
    }
}

/**
 * Áp dụng trọng lực theo chiều dọc cho actor.
 *
 * Tốc độ rơi được giới hạn để tránh bước di chuyển mỗi frame quá lớn làm xử lý
 * va chạm mất ổn định.
 */
void Actor::applyGravity() {
    velocityY = std::min(velocityY + kGravity, kMaxFallSpeed);
}

/**
 * Xử lý va chạm với game object rắn bằng độ chồng lấp AABB.
 *
 * Hàm này chỉ thực hiện tách vật lý chung. Các quyết định riêng của game như
 * nhặt item, sát thương kẻ địch, điểm số hoặc chạm cờ nên nằm trong
 * CollisionSystem hoặc lớp gameplay liên quan.
 *
 * @param object Object cần tách khỏi actor này.
 */
void Actor::resolveCollision(GameObject& object) {
    const Rectangle self = getBounds();
    const Rectangle other = object.getBounds();

    if (!intersects(self, other)) {
        return;
    }

    const double selfCenterX = self.x + self.width / 2.0;
    const double selfCenterY = self.y + self.height / 2.0;
    const double otherCenterX = other.x + other.width / 2.0;
    const double otherCenterY = other.y + other.height / 2.0;

    const double overlapLeft = self.x + self.width - other.x;
    const double overlapRight = other.x + other.width - self.x;
    const double overlapTop = self.y + self.height - other.y;
    const double overlapBottom = other.y + other.height - self.y;

    const double overlapX = std::min(overlapLeft, overlapRight);
    const double overlapY = std::min(overlapTop, overlapBottom);

    if (overlapX < overlapY) {
        x += (selfCenterX < otherCenterX) ? -overlapX : overlapX;
        velocityX = 0.0;
    } else {
        y += (selfCenterY < otherCenterY) ? -overlapY : overlapY;
        velocityY = 0.0;
    }
}
