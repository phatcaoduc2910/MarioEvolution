#pragma once 

#include "GameObject.h"

class Player;

// Lớp Item
class Item : public GameObject{
public:
    // Khởi tạo vị trí, kích thước, vùng va chạm
    Item(double x, double y, int width, int height);
    ~Item() override = default;

    bool isCollected() const;
    void applyTo(Player& player);

protected:
    virtual void applyEffect(Player& player) = 0;

private:
    bool collected;
};

// Lớp Coin
class Coin : public Item{
public:
    // Tạo đồng xu
    Coin(double x, double y, int value);

    int getValue() const;   // Trả về số điểm của đồng xu

private:
    void applyEffect(Player& player) override;

    int value;
};

// lớp Mushroom: chuyển người chơi sang trạng thái Big
class Mushroom : public Item{
public:
    Mushroom(double x, double y);

private:
    void applyEffect(Player& player) override;
};

// Lớp FireFlower: chuyển người chơi sang trạng thái Fire
class FireFlower : public Item{
public:
    FireFlower(double x, double y);

private:
    void applyEffect(Player& player) override;
};
