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
    virtual void applyTo(Player& player);
    virtual void collect();

protected:
    bool collected;
};

// Lớp Coin
class Coin : public Item{
public:
    // Tạo đồng xu
    Coin(double x, double y, int value);

    int getValue() const;   // Trả về số điểm của đồng xu
    void applyTo(Player& player) override;

private:
    int value;
};

// lớp Mushroom: chuyển người chơi sang trạng thái Big
class Mushroom : public Item{
public:
    Mushroom(double x, double y);

    void applyTo(Player& player) override;
};

// Lớp FireFlower: chuyển người chơi sang trạng thái Fire
class FireFlower : public Item{
public:
    FireFlower(double x, double y);

    void applyTo(Player& player) override;
};