#pragma once

#include "StaticObject.h"

#include <memory>

class Item;
class Player;

// Lớp cơ sở Brick
class Brick : public StaticObject{
public:
    Brick(double x, double y, bool breakable);
    ~Brick() override = default;

    virtual void hitBy(Player& player);     // Xử lý khi Player đập gạch
    void update(double dtSeconds) override;
    bool canBeBroken() const;
    bool isOpened() const;
    int getRenderOffsetY() const;

protected:
    bool isActive() const;
    void markUsed();

private:
    enum class State {
        Active,
        Broken,
        Used
    };

    void markBroken();
    void startBump();

    bool breakable;
    State state;
    double bumpElapsedSeconds;
    int renderOffsetY;
    bool bumping;
};

// Brick thường
class StandardBrick : public Brick{
public:
    StandardBrick(double x, double y);
};

// Brick chứa item
class SpecialBrick : public Brick{
public:
    ~SpecialBrick() override = default;

    virtual std::unique_ptr<Item> releaseItem() = 0;

protected:
    SpecialBrick(double x, double y);
};

// Brick chứa coin
class CoinBrick : public SpecialBrick{
public:
    CoinBrick(double x, double y, int coinAmount);

    std::unique_ptr<Item> releaseItem() override;

private:
    int coinAmount;
};

// Brick chứa một Mushroom
class MushroomBrick : public SpecialBrick{
public:
    MushroomBrick(double x, double y);

    std::unique_ptr<Item> releaseItem() override;
};

// Brick chứa FireFlower, tương tự Mushroom
class FlowerBrick : public SpecialBrick{
public:
    FlowerBrick(double x, double y);

    std::unique_ptr<Item> releaseItem() override;
};
