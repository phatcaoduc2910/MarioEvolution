#pragma once

#include "Actor.h"

class Flag;
class Item;

class Player : public Actor {
public:
    static constexpr int kBodyWidth = 32;
    static constexpr int kSmallHeight = 32;
    static constexpr int kBigHeight = 54;

    Player(double x, double y);

    PowerUpType getPowerUp() const;
    PlayerState getState() const;
    bool isAlive() const override;
    bool isInvincible() const;

    void jump();
    void bounceAfterStomp();
    void setMoveDirection(int direction);
    void collect(Item& item);
    void grow();
    void upgradeToFire();
    void takeDamage();
    void captureFlag(Flag& flag);
    void shootFireball();

    void update(double dtSeconds) override;
    void render() override;

private:
    void startInvincibility();
    void resizeForState();

    PlayerState state;
    int moveDirection;
    double invincibilityRemainingSeconds;
};
