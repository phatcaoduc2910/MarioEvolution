#pragma once

#include "GameObject.h"

class Player;

class Item : public GameObject {
public:
    Item(double x, double y, int width, int height);
    ~Item() override = default;

    bool isCollected() const;
    virtual void applyTo(Player& player);
    virtual void collect();

protected:
    bool collected;
};

class Coin : public Item {
public:
    Coin(double x, double y, int value);

    int getValue() const;
    void applyTo(Player& player) override;

private:
    int value;
};

class Mushroom : public Item {
public:
    Mushroom(double x, double y);

    void applyTo(Player& player) override;
};

class FireFlower : public Item {
public:
    FireFlower(double x, double y);

    void applyTo(Player& player) override;
};
