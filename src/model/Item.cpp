#include "model/Item.h"

#include "model/Player.h"

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
      value(value) {}

int Coin::getValue() const {
    return value;
}

void Coin::applyTo(Player& player) {
    (void)player;
}

Mushroom::Mushroom(double x, double y)
    : Item(x, y, 32, 32) {}

void Mushroom::applyTo(Player& player) {
    (void)player;
}

FireFlower::FireFlower(double x, double y)
    : Item(x, y, 32, 32) {}

void FireFlower::applyTo(Player& player) {
    (void)player;
}
