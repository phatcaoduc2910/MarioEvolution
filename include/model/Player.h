#pragma once

#include "Actor.h"

class Flag;
class Item;

/**
 * Actor trung tâm nhận input, vật phẩm, sát thương và điều kiện thắng.
 */
class Player : public Actor {
public:
    Player(double x, double y);

    PowerUpType getPowerUp() const;
    PlayerState getState() const;
    bool isInvincible() const;

    void jump();
    void setMoveDirection(int direction);
    void collect(Item& item);
    void takeDamage();
    void captureFlag(Flag& flag);
    void shootFireball();

    void update() override;
    void render() override;

private:
    PowerUpType powerUp;
    PlayerState state;
    bool invincible;
};
