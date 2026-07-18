#pragma once

#include "StaticObject.h"

#include <memory>

class Item;
class Player;

/**
 * Nền chung cho brick có trạng thái breakable và opened.
 */
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

/**
 * Brick thường có thể bị phá bởi player đủ sức mạnh.
 */
class StandardBrick : public Brick {
public:
    StandardBrick(double x, double y);

    void breakBrick();
};

/**
 * Brick chứa item và chỉ giải phóng nội dung một lần.
 */
class SpecialBrick : public Brick {
public:
    SpecialBrick(double x, double y, ItemType content);
    ~SpecialBrick() override = default;

    virtual std::unique_ptr<Item> releaseItem();
    void open();

protected:
    ItemType content;
};

/**
 * Brick giải phóng một số lượng coin hữu hạn.
 */
class CoinBrick : public SpecialBrick {
public:
    CoinBrick(double x, double y, int coinAmount);

    std::unique_ptr<Item> releaseItem() override;

private:
    int coinAmount;
};

/**
 * Brick giải phóng một mushroom khi được đập lần đầu.
 */
class MushroomBrick : public SpecialBrick {
public:
    MushroomBrick(double x, double y);

    std::unique_ptr<Item> releaseItem() override;
};

/**
 * Brick giải phóng một fire flower khi được đập lần đầu.
 */
class FlowerBrick : public SpecialBrick {
public:
    FlowerBrick(double x, double y);

    std::unique_ptr<Item> releaseItem() override;
};
