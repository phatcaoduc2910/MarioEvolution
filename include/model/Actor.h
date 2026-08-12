#pragma once

#include "GameObject.h"

class Actor : public GameObject {
public:
    Actor(double x, double y, int width, int height);
    ~Actor() override = default;

    virtual bool isAlive() const;
    bool isOnGround() const;
    Direction getDirection() const;
    double getVelocityX() const;
    double getVelocityY() const;

    virtual void move(double dtSeconds);
    virtual void applyGravity(double dtSeconds);
    virtual void resolveCollision(GameObject& object);

protected:
    double velocityX;
    double velocityY;
    Direction direction;
    bool alive;
    bool onGround;
};
