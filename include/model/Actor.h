#pragma once

#include "GameObject.h"

/**
 * GameObject có chuyển động, hướng nhìn và trạng thái sống.
 */
class Actor : public GameObject {
public:
    Actor(double x, double y, int width, int height);
    ~Actor() override = default;

    bool isAlive() const;
    bool isOnGround() const;
    Direction getDirection() const;
    double getVelocityX() const;
    double getVelocityY() const;

    virtual void move();
    virtual void applyGravity();
    virtual void resolveCollision(GameObject& object);

protected:
    double velocityX;
    double velocityY;
    Direction direction;
    bool alive;
    bool onGround;
};
