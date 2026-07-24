#include "model/Brick.h"

#include "model/Item.h"
#include "model/Player.h"

#include <algorithm>

Brick::Brick(double x, double y, bool breakable)
    : StaticObject(x, y, 32, 32),
      breakable(breakable),
      opened(false) {}

void Brick::hitBy(Player& player) {
    if (opened) {
        return;
    }

    if (breakable) {
        if (player.getState() != PlayerState::Small &&
            player.getState() != PlayerState::Dead) {
            opened = true;
            solid = false;
        }
        return;
    }

}


bool Brick::canBeBroken() const {
    return breakable;
}


bool Brick::isOpened() const {
    return opened;
}

StandardBrick::StandardBrick(double x, double y)
    : Brick(x, y, true) {}

void StandardBrick::breakBrick() {
    if (!opened) {
        opened = true;
        solid = false;
    }
}

SpecialBrick::SpecialBrick(double x, double y, ItemType content)
    : Brick(x, y, false),
      content(content) {}

std::unique_ptr<Item> SpecialBrick::releaseItem() {
    if (opened) {
        return nullptr;
    }

    std::unique_ptr<Item> item;
    switch (content) {
        case ItemType::Coin:
            item = std::make_unique<Coin>(x + (width - 16) / 2.0, y - 16, 1);
            break;
        case ItemType::Mushroom:
            item = std::make_unique<Mushroom>(x, y - 32);
            break;
        case ItemType::FireFlower:
            item = std::make_unique<FireFlower>(x, y - 32);
            break;
    }

    open();
    return item;
}

void SpecialBrick::open() {
    opened = true;
}

CoinBrick::CoinBrick(double x, double y, int coinAmount)
    : SpecialBrick(x, y, ItemType::Coin),
      coinAmount(std::max(0, coinAmount)) {}

std::unique_ptr<Item> CoinBrick::releaseItem() {
    if (coinAmount <= 0) {
        open();
        return nullptr;
    }

    auto coin = std::make_unique<Coin>(x + (width - 16) / 2.0, y - 16, 1);
    --coinAmount;
    if (coinAmount == 0) {
        open();
    }
    return coin;
}

MushroomBrick::MushroomBrick(double x, double y)
    : SpecialBrick(x, y, ItemType::Mushroom) {}

std::unique_ptr<Item> MushroomBrick::releaseItem() {
    if (opened) {
        return nullptr;
    }

    auto mushroom = std::make_unique<Mushroom>(x, y - 32);
    open();
    return mushroom;
}

FlowerBrick::FlowerBrick(double x, double y)
    : SpecialBrick(x, y, ItemType::FireFlower) {}

std::unique_ptr<Item> FlowerBrick::releaseItem() {
    if (opened) {
        return nullptr;
    }

    auto flower = std::make_unique<FireFlower>(x, y - 32);
    open();
    return flower;
}
