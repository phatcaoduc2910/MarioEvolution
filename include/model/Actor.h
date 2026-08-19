#pragma once

#include "GameObject.h"

#include <cstdint>

class Actor : public GameObject {
public:
    Actor(double x, double y, int width, int height);
    ~Actor() override = default;

    // Id duy nhất theo phiên chạy: boss dùng nó để nhớ đã quyết định né hay
    // chưa cho từng shell, con trỏ có thể bị cấp phát lại nên không tin được.
    std::uint32_t getId() const;

    virtual bool isAlive() const;
    virtual bool isRemovable() const;
    bool isOnGround() const;
    Direction getDirection() const;
    double getVelocityX() const;
    double getVelocityY() const;
    double getLandingImpactSpeed() const;

    virtual void applyGravity(double dtSeconds);

    // Đi từng trục một để CollisionSystem resolve xong trục này mới sang trục kia.
    void moveX(double dtSeconds);
    void moveY(double dtSeconds);

    void placeBesideWall(double colliderX);
    void placeOnGround(double colliderY);
    void placeUnderCeiling(double colliderY);

protected:
    std::uint32_t id;
    double velocityX;
    double velocityY;
    Direction direction;
    bool alive;
    bool onGround;
    double landingImpactSpeed;
    bool groundedBeforeStep;
};
