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

    virtual void applyGravity(double dtSeconds);

    // Đi từng trục một để CollisionSystem resolve xong trục này mới sang trục kia.
    void moveX(double dtSeconds);
    void moveY(double dtSeconds);

    // CollisionSystem tính chỗ tách rồi giao lại; Actor tự áp state của mình.
    void placeBesideWall(double newX);
    void placeOnGround(double newY);
    void placeUnderCeiling(double newY);

protected:
    double velocityX;
    double velocityY;
    Direction direction;
    bool alive;
    bool onGround;
};
