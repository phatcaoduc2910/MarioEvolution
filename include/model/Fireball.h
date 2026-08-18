#pragma once

#include "Actor.h"

class Fireball : public Actor {
public:
    static constexpr int kSize = 16;

    Fireball(double x, double y, Direction direction);

    void update(double dtSeconds) override;
    void bounce();
    void destroy();

private:
    bool hasBounced;
};
