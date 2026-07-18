#include "model/Item.h"

#include "model/Player.h"

#include <algorithm>

/**
 * Khởi tạo item chưa được thu thập.
 *
 * @param x Tọa độ x của item.
 * @param y Tọa độ y của item.
 * @param width Chiều rộng vùng va chạm.
 * @param height Chiều cao vùng va chạm.
 */
Item::Item(double x, double y, int width, int height)
    : GameObject(x, y, width, height),
      collected(false) {}

/**
 * @return true nếu item đã được thu thập; ngược lại là false.
 */
bool Item::isCollected() const {
    return collected;
}

/**
 * Điểm mở rộng để lớp item cụ thể áp dụng hiệu ứng lên player.
 *
 * @param player Player nhận hiệu ứng.
 */
void Item::applyTo(Player& player) {
    (void)player;
}

/**
 * Đánh dấu item đã được thu thập.
 */
void Item::collect() {
    collected = true;
}

/**
 * Khởi tạo coin 16x16 với giá trị không âm.
 *
 * @param x Tọa độ x của coin.
 * @param y Tọa độ y của coin.
 * @param value Số điểm của coin.
 */
Coin::Coin(double x, double y, int value)
    : Item(x, y, 16, 16),
      value(std::max(0, value)) {}

/**
 * @return Giá trị điểm của coin.
 */
int Coin::getValue() const {
    return value;
}

/**
 * Giữ hook hiệu ứng coin để các item có cùng giao diện.
 *
 * Điểm của coin được World cộng trong CollisionSystem.
 *
 * @param player Player thu thập coin.
 */
void Coin::applyTo(Player& player) {
    // Điểm của đồng xu do World quản lý; Player không có API cập nhật điểm.
    (void)player;
}

/**
 * Khởi tạo mushroom có kích thước 32x32.
 *
 * @param x Tọa độ x của mushroom.
 * @param y Tọa độ y của mushroom.
 */
Mushroom::Mushroom(double x, double y)
    : Item(x, y, 32, 32) {}

/**
 * Giữ hook hiệu ứng mushroom để Player::collect áp dụng power-up.
 *
 * @param player Player thu thập mushroom.
 */
void Mushroom::applyTo(Player& player) {
    // Player::collect nhận vật phẩm và áp dụng nâng cấp tương ứng.
    (void)player;
}

/**
 * Khởi tạo fire flower có kích thước 32x32.
 *
 * @param x Tọa độ x của fire flower.
 * @param y Tọa độ y của fire flower.
 */
FireFlower::FireFlower(double x, double y)
    : Item(x, y, 32, 32) {}

/**
 * Giữ hook hiệu ứng fire flower để Player::collect áp dụng power-up.
 *
 * @param player Player thu thập fire flower.
 */
void FireFlower::applyTo(Player& player) {
    // Player::collect nhận vật phẩm và áp dụng nâng cấp tương ứng.
    (void)player;
}
