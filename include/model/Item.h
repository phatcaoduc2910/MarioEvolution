#pragma once

#include "GameObject.h"

class Player;

/**
 * GameObject có thể được player thu thập một lần.
 */
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

/**
 * Đồng xu có giá trị điểm do World cộng khi thu thập.
 */
class Coin : public Item {
public:
    Coin(double x, double y, int value);

    int getValue() const;
    void applyTo(Player& player) override;

private:
    int value;
};

/**
 * Power-up chuyển small player sang trạng thái lớn.
 */
class Mushroom : public Item {
public:
    Mushroom(double x, double y);

    void applyTo(Player& player) override;
};

/**
 * Power-up cấp trạng thái bắn lửa cho player.
 */
class FireFlower : public Item {
public:
    FireFlower(double x, double y);

    void applyTo(Player& player) override;
};
