#include "model/Brick.h"

#include "model/Item.h"
#include "model/Player.h"

Brick::Brick(double x, double y, bool breakable)
    : StaticObject(x, y, 32, 32),
      breakable(breakable),
      opened(false) {}

void Brick::hitBy(Player& player) {
    (void)player;
}

bool Brick::canBeBroken() const {
    return breakable;
}

bool Brick::isOpened() const {
    return opened;
}

StandardBrick::StandardBrick(double x, double y)
    : Brick(x, y, true) {}

void StandardBrick::breakBrick() {}

SpecialBrick::SpecialBrick(double x, double y, ItemType content)
    : Brick(x, y, false),
      content(content) {}

std::unique_ptr<Item> SpecialBrick::releaseItem() {
    return nullptr;
}

void SpecialBrick::open() {}

CoinBrick::CoinBrick(double x, double y, int coinAmount)
    : SpecialBrick(x, y, ItemType::Coin),
      coinAmount(coinAmount) {}

std::unique_ptr<Item> CoinBrick::releaseItem() {
    return nullptr;
}

MushroomBrick::MushroomBrick(double x, double y)
    : SpecialBrick(x, y, ItemType::Mushroom) {}

std::unique_ptr<Item> MushroomBrick::releaseItem() {
    return nullptr;
}

FlowerBrick::FlowerBrick(double x, double y)
    : SpecialBrick(x, y, ItemType::FireFlower) {}

std::unique_ptr<Item> FlowerBrick::releaseItem() {
    return nullptr;
}
