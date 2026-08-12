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
    if (collected) {
        return;
    }

    applyEffect(player);
    collected = true;
}

Coin::Coin(double x, double y, int value)
    : Item(x, y, 16, 16),
      value(std::max(0, value)) {}


int Coin::getValue() const {
    return value;
}

void Coin::applyEffect(Player& player) {
    // Score thuộc World nên CollisionSystem cộng điểm, Coin chỉ dùng chung hook.
    (void)player;
}

Mushroom::Mushroom(double x, double y)
    : Item(x, y, 32, 32) {}

void Mushroom::applyEffect(Player& player) {
    player.grow();
}

FireFlower::FireFlower(double x, double y)
    : Item(x, y, 32, 32) {}

void FireFlower::applyEffect(Player& player) {
    player.upgradeToFire();
}
