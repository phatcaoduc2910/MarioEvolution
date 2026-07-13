#include "model/Item.h"

#include "model/Player.h"

#include <algorithm>

Item::Item(double x, double y, int width, int height)
    : GameObject(x, y, width, height),
      collected(false) {}

bool Item::isCollected() const {
    return collected;
}

void Item::applyTo(Player& player) {
    (void)player;
}

void Item::collect() {
    collected = true;
}

Coin::Coin(double x, double y, int value)
    : Item(x, y, 16, 16),
      value(std::max(0, value)) {}

int Coin::getValue() const {
    return value;
}

void Coin::applyTo(Player& player) {
    // Điểm của đồng xu do World quản lý; Player không có API cập nhật điểm.
    (void)player;
}

Mushroom::Mushroom(double x, double y)
    : Item(x, y, 32, 32) {}

void Mushroom::applyTo(Player& player) {
    // Player::collect nhận vật phẩm và áp dụng nâng cấp tương ứng.
    (void)player;
}

FireFlower::FireFlower(double x, double y)
    : Item(x, y, 32, 32) {}

void FireFlower::applyTo(Player& player) {
    // Player::collect nhận vật phẩm và áp dụng nâng cấp tương ứng.
    (void)player;
}
