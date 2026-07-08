#pragma once

#include "GameObject.h"

class Actor : public GameObject {
public:
    Actor(double x, double y, int width, int height);
    ~Actor() override = default;

    bool isAlive() const;
    Direction getDirection() const;

    virtual void move();
    virtual void applyGravity();
    virtual void resolveCollision(GameObject& object);

protected:
    double velocityX;
    double velocityY;
    Direction direction;
    bool alive;
};
