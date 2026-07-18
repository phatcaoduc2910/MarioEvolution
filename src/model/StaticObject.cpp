#include "model/StaticObject.h"

#include "model/Actor.h"

/**
 * Khởi tạo object tĩnh có va chạm rắn.
 *
 * @param x Tọa độ x của object.
 * @param y Tọa độ y của object.
 * @param width Chiều rộng vùng va chạm.
 * @param height Chiều cao vùng va chạm.
 */
StaticObject::StaticObject(double x, double y, int width, int height)
    : GameObject(x, y, width, height),
      solid(true) {}

/**
 * @return true nếu object đang chặn actor; ngược lại là false.
 */
bool StaticObject::isSolid() const {
    return solid;
}

/**
 * Yêu cầu actor tự tách khỏi object khi object còn rắn.
 *
 * @param actor Actor đang va chạm với object.
 */
void StaticObject::onCollision(Actor& actor) {
    if (solid) {
        actor.resolveCollision(*this);
    }
}
