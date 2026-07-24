#pragma once

#include "StaticObject.h"

#include <memory>

class Item;
class Player;

class Brick : public StaticObject {
public:
    Brick(double x, double y, bool breakable);
    ~Brick() override = default;

    virtual void hitBy(Player& player);
    bool canBeBroken() const;
    bool isOpened() const;

protected:
    bool breakable;
    bool opened;
};

class StandardBrick : public Brick {
public:
    StandardBrick(double x, double y);

    void breakBrick();
};

class SpecialBrick : public Brick {
public:
    SpecialBrick(double x, double y, ItemType content);
    ~SpecialBrick() override = default;

    virtual std::unique_ptr<Item> releaseItem();
    void open();

protected:
    ItemType content;
};

class CoinBrick : public SpecialBrick {
public:
    CoinBrick(double x, double y, int coinAmount);

    std::unique_ptr<Item> releaseItem() override;

private:
    int coinAmount;
};

class MushroomBrick : public SpecialBrick {
public:
    MushroomBrick(double x, double y);

    std::unique_ptr<Item> releaseItem() override;
};

class FlowerBrick : public SpecialBrick {
public:
    FlowerBrick(double x, double y);

    std::unique_ptr<Item> releaseItem() override;
};
