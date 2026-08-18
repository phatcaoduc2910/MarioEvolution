#pragma once

#include "Actor.h"

class Flag;
class Item;

class Player : public Actor {
public:
    static constexpr int kBodyWidth = 32;
    static constexpr int kSmallHeight = 32;
    static constexpr int kBigHeight = 54;

    static constexpr int kColliderWidth = 20;
    static constexpr int kColliderInsetX = (kBodyWidth - kColliderWidth) / 2;
    static constexpr int kColliderInsetTop = 4;

    static constexpr int kFeetInsetX = 2;
    static constexpr int kFeetHeight = 3;

    Player(double x, double y);

    PowerUpType getPowerUp() const;
    PlayerState getState() const;
    bool isAlive() const override;
    bool isInvincible() const;

    Rectangle getBounds() const override;
    Rectangle getFeetBounds() const;

    void jump();
    void bounceAfterStomp();
    void setMoveDirection(int direction);
    void collect(Item& item);
    void grow();
    void upgradeToFire();
    void takeDamage();
    void reviveAt(double reviveX, double reviveY);
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
